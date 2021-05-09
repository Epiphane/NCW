// By Thomas Steinke

#include <queue>
#include <mutex>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <RGBDesignPatterns/Macros.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBLogger/Logger.h>
#include <Engine/Core/FileSystemProvider.h>
#include <Engine/Core/Timer.h>
#include <Shared/Helpers/Asset.h>
#include <Engine/Script/JSScript.h>
#include <Shared/DebugHelper.h>

#include "ChunkGenerator.h"

namespace CubeWorld
{

noise::module::Perlin gHeightmodule;
noise::utils::NoiseMap gHeightmap;

struct ChunkGenerator::RequestQueue
{
    // Protects the queue of requests.
    std::mutex mutex;

    // Queued requests for generation.
    std::deque<Request> requests;

    // Condition variable which signals that a new request was queued.
    std::condition_variable condition;
};

class ChunkGenerator::Worker
{
public:
    //
    // Data owned by this specific thread.
    //
    struct PrivateData
    {
        // Worker thread
        std::thread thread;

        // Filename of the generator script
        std::string generatorFilename;

        // Current version of the source for the generator.
        std::string generatorSource;

        // Profiler
        Engine::Timer<1> profiler;
    };

    //
    // Data shared between all threads.
    //
    struct SharedData
    {
        // Protects the queue of requests.
        std::mutex mutex;

        // Queued requests for generation.
        std::deque<Request> requests;

        // Condition variable which signals that a new request was queued.
        std::condition_variable condition;

        // Protects the generator script.
        std::mutex generatorMutex;

        // Script for executing chunk creation.
        Engine::JSScript generator;

        // Whether the thread should exit.
        bool exiting = false;

        // Whether the thread has exited.
        bool exited = false;
    };

public:
    Worker(const std::string& sourceFile, Engine::EventManager& events)
        : mEvents(events)
    {
        gHeightmodule.SetFrequency(0.01f);
        gHeightmodule.SetOctaveCount(8);
        gHeightmodule.SetSeed(5);

        FileSystem& fs = Engine::FileSystemProvider::Instance();

        mShared.generator.SetEventManager(&events);
        mPrivate.generatorFilename = sourceFile;
        Maybe<std::string> source = fs.ReadEntireFile(sourceFile);
        if (!source)
        {
            source.Failure().WithContext("Failed reading script file").Log();
        }
        else
        {
            mPrivate.generatorSource = std::move(*source);
            mShared.generator.LoadSource(mPrivate.generatorSource, sourceFile);
        }

        {
            duk_context* ctx = mShared.generator.GetContext();
            DUK_GUARD_SCOPE();

            duk_get_global_string(ctx, "Game");
            duk_push_c_function(ctx, &Worker::GetNoise, 3);
            duk_put_prop_string(ctx, -2, "GetNoise");
            duk_pop(ctx);
        }

        mPrivate.thread = std::thread([this] { Run(); });
    }

    ~Worker()
    {
        Cancel();
        mPrivate.thread.join();
    }

    void Cancel()
    {
        std::unique_lock<std::mutex> lock{mShared.mutex};
        mShared.exiting = true;
        mShared.condition.notify_all();
        mShared.condition.wait(lock, [&] { return mShared.exited; });
    }

    void Run()
    {
        RunInternal();

        std::unique_lock<std::mutex> lock{mShared.mutex};
        mShared.exiting = true;
        mShared.exited = true;
        mShared.condition.notify_all();
    }

    void RunInternal()
    {
        for (;;)
        {
            Request request;

            {
                std::unique_lock<std::mutex> lock{mShared.mutex};
                mShared.condition.wait(lock, [&] {
                    return !mShared.requests.empty() || mShared.exiting;
                });

                if (mShared.exiting)
                {
                    return;
                }

                if (!mShared.requests.empty())
                {
                    request = std::move(mShared.requests.front());
                    mShared.requests.pop_front();
                }
            }

            BuildChunk(request);
        }
    }

    static duk_ret_t GetBlockValue(duk_context* ctx)
    {
        duk_idx_t nargs = duk_get_top(ctx);
        if (nargs != 2)
        {
            return DUK_RET_ERROR;
        }

        duk_require_object(ctx, 0);
        const char* key = duk_get_string(ctx, 1);

        duk_get_prop_string(ctx, 0, "_data");
        Block* data = static_cast<Block*>(duk_require_pointer(ctx, -1));

        // tiny optimization: we know that every property is one letter long.
        if (key[1] != 0)
        {
            return 0;
        }

        switch (key[0])
        {
        case 'r':
            duk_push_uint(ctx, data->r);
            return 1;
        case 'g':
            duk_push_uint(ctx, data->g);
            return 1;
        case 'b':
            duk_push_uint(ctx, data->b);
            return 1;
        case 'a':
            duk_push_uint(ctx, data->a);
            return 1;
        }
        
        return 0;
    }

    static duk_ret_t SetBlockValue(duk_context* ctx)
    {
        duk_idx_t nargs = duk_get_top(ctx);
        if (nargs != 3)
        {
            return DUK_RET_ERROR;
        }

        duk_require_object(ctx, 0);
        const char* key = duk_get_string(ctx, 1);
        uint32_t value_ = duk_require_uint(ctx, 2);
        if (value_ > std::numeric_limits<uint8_t>::max())
        {
            value_ = std::numeric_limits<uint8_t>::max();
            //return DUK_RET_RANGE_ERROR;
        }

        uint8_t value = uint8_t(value_);

        duk_get_prop_string(ctx, 0, "_data");
        Block* data = static_cast<Block*>(duk_require_pointer(ctx, -1));

        // tiny optimization: we know that every property is one letter long.
        switch (key[0])
        {
        case 'r':
            data->r = value;
            break;
        case 'g':
            data->g = value;
            break;
        case 'b':
            data->b = value;
            break;
        case 'a':
            data->a = value;
            break;
        }

        return 0;
    }

    static duk_ret_t GetBlock(duk_context* ctx)
    {
        duk_idx_t nargs = duk_get_top(ctx);
        if (nargs != 3)
        {
            return DUK_RET_ERROR;
        }

        duk_push_this(ctx);

        uint32_t x = duk_require_uint(ctx, 0);
        uint32_t y = duk_require_uint(ctx, 1);
        uint32_t z = duk_require_uint(ctx, 2);

        duk_get_prop_string(ctx, -1, "_data");
        Chunk* data = static_cast<Chunk*>(duk_require_pointer(ctx, -1));
        Block& block = data->Get(x, y, z);

        // Create a proxy for this block
        duk_push_object(ctx);   // Target

        duk_push_pointer(ctx, &block);          // Push pointer to block
        duk_put_prop_string(ctx, -2, "_data");  // Associate with _data property

        DUK_GUARD_SCOPE();      // The target should remain on the stack so we put the guard here.
        duk_push_object(ctx);   // Handler

        duk_push_c_function(ctx, &Worker::GetBlockValue, 2);    // Push get function
        duk_put_prop_string(ctx, -2, "get");                    // Associate as getter
        duk_push_c_function(ctx, &Worker::SetBlockValue, 3);    // Push set function
        duk_put_prop_string(ctx, -2, "set");                    // Associate as setter
        duk_push_proxy(ctx, 0);                                 // Associate the proxy handler with our returned object

        return 1;
    }

    static duk_ret_t GetNoise(duk_context* ctx)
    {
        double x = duk_require_number(ctx, 0);
        double y = duk_require_number(ctx, 1);
        double z = duk_require_number(ctx, 2);

        duk_push_number(ctx, gHeightmodule.GetValue(x, y, z));

        return 1;
    }

    void BuildChunk(const Request& request)
    {
        mPrivate.profiler.Reset();

        std::unique_lock<std::mutex> lock{ mShared.generatorMutex };

        Chunk chunk(request.coordinates);

        // Set up generation script with a Game.chunk, which proxies to this Chunk object.
        {
            duk_context* ctx = mShared.generator.GetContext();
            DUK_GUARD_SCOPE();

            duk_push_object(ctx);                           // Push chunk object

            duk_push_pointer(ctx, &chunk);                  // Push pointer to Chunk object
            duk_put_prop_string(ctx, -2, "_data");          // Put pointer under chunk._data
            duk_push_uint(ctx, kChunkSize);                 // Push Chunk width
            duk_put_prop_string(ctx, -2, "width");          // Assign chunk.width
            duk_push_uint(ctx, kChunkHeight);               // Push Chunk height
            duk_put_prop_string(ctx, -2, "height");         // Assign chunk.height
            duk_push_uint(ctx, kChunkSize);                 // Push Chunk depth
            duk_put_prop_string(ctx, -2, "depth");          // Assign chunk.depth

            duk_push_c_function(ctx, &Worker::GetBlock, 3); // Push GetBlock function. args: this, x, y, z
            duk_put_prop_string(ctx, -2, "Get");            // Put function under chunk.Get

            duk_put_global_string(ctx, "chunk");            // Push chunk into global context
        }

        mShared.generator.RunFunction(
            "Generate",
            request.coordinates.x,
            request.coordinates.y,
            request.coordinates.z
        );

        /*

        for (uint32_t y = 0; y < kChunkHeight; ++y)
        {
            for (uint32_t x = 0; x < kChunkSize; ++x)
            {
                for (uint32_t z = 0; z < kChunkSize; ++z)
                {
                    float globalX = float(x + chunkX * kChunkSize);
                    float globalY = float(y + chunkY * kChunkSize);
                    float globalZ = float(z + chunkZ * kChunkSize);
                    float noiseVal = float(gHeightmodule.GetValue(double(globalX), double(globalY), double(globalZ)));

                    CUBEWORLD_UNREFERENCED_VARIABLE(noiseVal);

                    auto& block = chunk.Get(x, y, z);
                    block.scale = -std::powf(std::max(globalY - 0.f, 0.f), 1.05f) / 32.0f;
                    if (globalY < 4)
                    {
                        block.scale /= (4.0f - globalY);
                    }
                    block.scale += noiseVal + 0.25f;
                }
            }
        }

        */

        DebugHelper::Instance().SetMetric("Chunk generation time", mPrivate.profiler.Elapsed());
        request.resultFunction(std::move(chunk));
    }

    void Add(const Request& request)
    {
        std::unique_lock<std::mutex> lock{mShared.mutex};
        mShared.requests.push_back(request);
        mShared.condition.notify_one();
    }

    void ClearQueue()
    {
        std::unique_lock<std::mutex> lock{ mShared.mutex };
        mShared.requests.clear();
    }

    void Update()
    {
        if (ImGui::Begin("Chunk Generator"))
        {
            ImVec2 size = ImGui::GetContentRegionAvail();
            size.y -= ImGui::GetItemsLineHeightWithSpacing();
            ImGui::InputTextMultiline("source", &mPrivate.generatorSource, size);
            if (ImGui::Button("Save and run"))
            {
                DiskFileSystem fs;
                if (auto result = fs.WriteFile(mPrivate.generatorFilename, mPrivate.generatorSource); !result)
                {
                    result.Failure().WithContext("Failed saving modified version of script").Log();
                }

                // Queue up the source to be rerun on the next worker invocation.
                mEvents.Emit<JavascriptEvent>("rebuild_world");
            }
        }
        ImGui::End();
    }

private:
    PrivateData mPrivate;
    SharedData mShared;

    Engine::EventManager& mEvents;
};

///
///
///
ChunkGenerator::ChunkGenerator(Engine::EventManager& events)
{
    mWorker.reset(new Worker(Asset::Script("chunk-generator.js"), events));
}

///
///
///
ChunkGenerator::~ChunkGenerator()
{
}

///
///
/// 
void ChunkGenerator::Add(const Request& request)
{
    mWorker->Add(request);
}

///
///
///
void ChunkGenerator::Clear()
{
    mWorker->ClearQueue();
}

/// 
/// 
/// 
void ChunkGenerator::Update()
{
    mWorker->Update();
}

}; // namespace CubeWorld

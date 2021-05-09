// By Thomas Steinke

#include <queue>
#include <mutex>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <RGBDesignPatterns/Macros.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBLogger/Logger.h>
#include <Engine/Core/Context.h>
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

        // Compute shader for chunk creation.
        std::unique_ptr<Engine::Graphics::Program> program;

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

        auto maybeProgram = Engine::Graphics::Program::LoadCompute(sourceFile);
        if (!maybeProgram)
        {
            maybeProgram.Failure().WithContext("Failed building compute shader").Log();
        }
        else
        {
            mShared.program = std::move(*maybeProgram);
        }

        mPrivate.thread = std::thread([this] {
            sContext.Activate();
            Engine::Graphics::VAO mVAO;
            mVAO.Bind();
            Run();
            sContext.Deactivate();
        });
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

    void BuildChunk(const Request& request)
    {
        mPrivate.profiler.Reset();

        std::unique_lock<std::mutex> lock{ mShared.generatorMutex };

        Chunk chunk(request.coordinates);

        Engine::Graphics::VBO vbo(Engine::Graphics::VBO::ShaderStorage);
        vbo.BufferData(chunk.data());

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

        {
            BIND_PROGRAM_IN_SCOPE(mShared.program);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo.GetBuffer());
            glDispatchCompute(2, 4, 2);

            // make sure writing to image has finished before read
            glMemoryBarrier(GL_ALL_BARRIER_BITS);

            // Get all the data back
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo.GetBuffer());
            GLsizeiptr size = GLsizeiptr(sizeof(Block) * chunk.data().size());
            glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, chunk.data().data());
        }

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

    // The graphics context is static, because we need it
    // to be set up when the program starts.
    static Engine::Context sContext;
};

Engine::Context ChunkGenerator::Worker::sContext;

///
///
///
ChunkGenerator::ChunkGenerator(Engine::EventManager& events)
{
    mWorker.reset(new Worker(Asset::Shader("ChunkGenerator.comp"), events));
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

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
        // Worker thread.
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
        std::mutex programMutex;

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
        Maybe<std::string> maybeSource = Engine::FileSystemProvider::Instance().ReadEntireFile(sourceFile);
        assert(maybeSource.Succeeded() && "Failed to read shader source");

        mPrivate.generatorFilename = sourceFile;
        mPrivate.generatorSource = std::move(*maybeSource);

        auto maybeProgram = Engine::Graphics::Program::LoadComputeSource(mPrivate.generatorSource);
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
        Engine::Graphics::VBO vbo(Engine::Graphics::VBO::ShaderStorage);

        Chunk chunk({ 0, 0, 0 });
        vbo.BufferData(chunk.data());

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

            BuildChunk(vbo, request);
        }
    }

    void BuildChunk(Engine::Graphics::VBO& vbo, const Request& request)
    {
        mPrivate.profiler.Reset();

        std::unique_lock<std::mutex> lock{ mShared.programMutex };

        Chunk chunk(request.coordinates);

        const float chunkX = float(request.coordinates.x);
        const float chunkY = float(request.coordinates.y);
        const float chunkZ = float(request.coordinates.z);

        if (mShared.program)
        {
            BIND_PROGRAM_IN_SCOPE(mShared.program);

            mShared.program->Uniform3f("uWorldCoords", chunkX, chunkY, chunkZ);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo.GetBuffer());
            glDispatchCompute(2, 3, 2);

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

// By Thomas Steinke

#include <chrono>
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

        // So we can chill tf out
        std::chrono::steady_clock::time_point lastUpdate;

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

        // Frequency divisor for noise, e.g. 1/128.0f
        float freqDivisor = 256.0f;

        // Number of octaves when generating noise.
        uint32_t octaves = 4;

        // Coordinate to base the world on
        float baseX = 0;
        float baseZ = 0;

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
        Engine::Graphics::VBO vbo;

        vbo.BufferData(Chunk::Size(), nullptr);

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

        std::unique_ptr<Chunk> chunk(new Chunk(request.coordinates));

        const float chunkX = (float(request.coordinates.x) - 0.5f) * kChunkSize;
        const float chunkY = (float(request.coordinates.y) - 0.5f) * kChunkHeight;
        const float chunkZ = (float(request.coordinates.z) - 0.5f) * kChunkSize;

        if (mShared.program)
        {
            BIND_PROGRAM_IN_SCOPE(mShared.program);
            mShared.program->Uniform3ui("uChunkSize", kChunkSize, kChunkHeight, kChunkSize);
            mShared.program->Uniform3f("uWorldCoords", chunkX, chunkY, chunkZ);
            mShared.program->Uniform3f("uWorldBase", mShared.baseX, 0, mShared.baseZ);
            mShared.program->Uniform1f("uFrequency", 1.0f / mShared.freqDivisor);
            mShared.program->Uniform1ui("uOctaves", mShared.octaves);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo.GetBuffer());
            glDispatchCompute(kChunkSize / 8, kChunkHeight / 16, kChunkSize / 8);

            // make sure writing to image has finished before read
            glMemoryBarrier(GL_ALL_BARRIER_BITS);

            // Get all the data back
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo.GetBuffer());
            glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, GLsizeiptr(Chunk::Size()), chunk->data().data());
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
            if (ImGui::BeginTabBar("Generator Properties"))
            {
                if (ImGui::BeginTabItem("Source"))
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

                        std::unique_lock<std::mutex> lock{ mShared.programMutex };
                        auto maybeProgram = Engine::Graphics::Program::LoadComputeSource(mPrivate.generatorSource);
                        if (!maybeProgram)
                        {
                            maybeProgram.Failure().WithContext("Failed building compute shader").Log();
                        }
                        else
                        {
                            mShared.program = std::move(*maybeProgram);
                        }

                        // Queue up the source to be rerun on the next worker invocation.
                        mEvents.Emit<JavascriptEvent>("rebuild_world");
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Properties"))
                {
                    std::unique_lock<std::mutex> lock{ mShared.programMutex };

                    std::chrono::steady_clock::duration timeSinceLastUpdate = std::chrono::steady_clock::now() - mPrivate.lastUpdate;
                    bool canUpdate = timeSinceLastUpdate > std::chrono::milliseconds(120);

                    if ((ImGui::SliderFloat("Frequency divisor", &mShared.freqDivisor, 1.0f, 1024.0f, "%.1f", 2.0f) && canUpdate) ||
                        ImGui::IsItemDeactivatedAfterChange())
                    {
                        mEvents.Emit<JavascriptEvent>("rebuild_world");
                        mPrivate.lastUpdate = std::chrono::steady_clock::now();
                    }

                    if ((ImGui::SliderInt("Octaves", (int*)&mShared.octaves, 1, 16) && canUpdate) ||
                        ImGui::IsItemDeactivatedAfterChange())
                    {
                        mEvents.Emit<JavascriptEvent>("rebuild_world");
                        mPrivate.lastUpdate = std::chrono::steady_clock::now();
                    }

                    if ((ImGui::DragFloat("Base X", &mShared.baseX, 0.01f) && canUpdate) ||
                        ImGui::IsItemDeactivatedAfterChange())
                    {
                        mEvents.Emit<JavascriptEvent>("rebuild_world");
                        mPrivate.lastUpdate = std::chrono::steady_clock::now();
                    }

                    if ((ImGui::DragFloat("Base Z", &mShared.baseZ, 0.01f) && canUpdate) ||
                        ImGui::IsItemDeactivatedAfterChange())
                    {
                        mEvents.Emit<JavascriptEvent>("rebuild_world");
                        mPrivate.lastUpdate = std::chrono::steady_clock::now();
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
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

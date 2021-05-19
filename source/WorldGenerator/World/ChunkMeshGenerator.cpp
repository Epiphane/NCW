// By Thomas Steinke

#include <queue>
#include <mutex>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <Engine/Core/FileSystemProvider.h>
#include <Engine/Core/Context.h>
#include <Engine/Core/Timer.h>
#include <Engine/Script/JSScript.h>
#include <Shared/Helpers/Asset.h>
#include <RGBDesignPatterns/Macros.h>

#include "ChunkMeshGenerator.h"

namespace CubeWorld
{

struct ChunkMeshGenerator::RequestQueue
{
    // Protects the queue of requests.
    std::mutex mutex;

    // Queued requests for generation.
    std::deque<Request> requests;

    // Condition variable which signals that a new request was queued.
    std::condition_variable condition;
};

class ChunkMeshGenerator::Worker
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
        std::unique_lock<std::mutex> lock{ mShared.mutex };
        mShared.exiting = true;
        mShared.condition.notify_all();
        mShared.condition.wait(lock, [&] { return mShared.exited; });
    }

    void Run()
    {
        RunInternal();

        std::unique_lock<std::mutex> lock{ mShared.mutex };
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
                std::unique_lock<std::mutex> lock{ mShared.mutex };
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

            BuildMesh(request);
        }
    }

    void BuildMesh(const Request& request)
    {
        mPrivate.profiler.Reset();

        std::unique_lock<std::mutex> lock{ mShared.programMutex };

        Engine::Graphics::VBO input;
        input.BufferData(request.chunk->data());

        uint32_t count[2] = { 0, 0 };

        // Allocate space for for every potential block in the chunk.
        Engine::Graphics::VBO vertices, colors, normals, indices;
        vertices.BufferData(sizeof(glm::vec4) * 4 * kChunkSize * kChunkSize * kChunkHeight, nullptr, GL_DYNAMIC_DRAW);
        colors.BufferData(sizeof(glm::vec4) * 4 * kChunkSize * kChunkSize * kChunkHeight, nullptr, GL_DYNAMIC_DRAW);
        normals.BufferData(sizeof(glm::vec4) * 4 * kChunkSize * kChunkSize * kChunkHeight, nullptr, GL_DYNAMIC_DRAW);
        indices.BufferData(sizeof(glm::tvec4<GLuint>) * 8 * kChunkSize * kChunkSize * kChunkHeight, nullptr, GL_DYNAMIC_DRAW);

        // declare and generate a buffer object name
        Engine::Graphics::VBO atomics;
        // bind the buffer and define its initial storage capacity
        atomics.BufferData(sizeof(GLuint) * 2, count, GL_DYNAMIC_DRAW);

        if (mShared.program)
        {
            BIND_PROGRAM_IN_SCOPE(mShared.program);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input.GetBuffer());
            glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, atomics.GetBuffer());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vertices.GetBuffer());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, colors.GetBuffer());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, normals.GetBuffer());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, indices.GetBuffer());
            glDispatchCompute(2, 3, 2);

            // make sure writing to the buffers has finished
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
        }

        atomics.Bind(VBOTarget::VertexData);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(count), count);

        Engine::Graphics::VBO vertices2, colors2, normals2, indices2;
        vertices2.CopyFrom(vertices, sizeof(glm::vec4) * count[0]);
        colors2.CopyFrom(colors, sizeof(glm::vec4) * count[0]);
        normals2.CopyFrom(normals, sizeof(glm::vec4) * count[0]);
        indices2.CopyFrom(indices, sizeof(GLuint) * 4 * count[1]);
        glFlush();

        request.component->Set(
            std::move(vertices2),
            std::move(colors2),
            std::move(normals2),
            std::move(indices2),
            count[0], size_t(count[1]) * 4
        );

        DebugHelper::Instance().SetMetric("Mesh generation time", mPrivate.profiler.Elapsed());
        request.resultFunction();
    }

    void Add(const Request& request)
    {
        std::unique_lock<std::mutex> lock{ mShared.mutex };
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

Engine::Context ChunkMeshGenerator::Worker::sContext;

///
///
///
ChunkMeshGenerator::ChunkMeshGenerator(Engine::EventManager& events)
{
    mWorker.reset(new Worker(Asset::Shader("ChunkMeshGenerator.comp"), events));
}

///
///
///
ChunkMeshGenerator::~ChunkMeshGenerator()
{
}

///
///
///
void ChunkMeshGenerator::Add(const Request& request)
{
    mWorker->Add(request);
}

///
///
///
void ChunkMeshGenerator::Clear()
{
    mWorker->ClearQueue();
}

///
///
///
void ChunkMeshGenerator::Update()
{
    mWorker->Update();
}

}; // namespace CubeWorld

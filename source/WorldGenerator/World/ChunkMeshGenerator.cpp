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
        mPrivate.generatorFilename = sourceFile;
        LoadShader();

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

    void LoadShader()
    {
        std::unique_lock<std::mutex> lock{ mShared.programMutex };

        auto maybeProgram = Engine::Graphics::Program::LoadCompute(mPrivate.generatorFilename);
        if (!maybeProgram)
        {
            maybeProgram.Failure().WithContext("Failed building compute shader").Log();
            mLastError = maybeProgram.Failure().GetMessage();
        }
        else
        {
            mShared.program = std::move(*maybeProgram);
            mLastError = "";
        }
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
        Engine::Graphics::VBO atomics, vertices, colors, normals, indices;
        vertices.BufferData(sizeof(glm::vec4) * 4 * kChunkSize * kChunkSize * kChunkHeight, nullptr, GL_DYNAMIC_DRAW);
        colors.BufferData(sizeof(glm::vec4) * 4 * kChunkSize * kChunkSize * kChunkHeight, nullptr, GL_DYNAMIC_DRAW);
        normals.BufferData(sizeof(glm::vec4) * 4 * kChunkSize * kChunkSize * kChunkHeight, nullptr, GL_DYNAMIC_DRAW);
        indices.BufferData(sizeof(glm::tvec4<GLuint>) * 8 * kChunkSize * kChunkSize * kChunkHeight, nullptr, GL_DYNAMIC_DRAW);

        Engine::Graphics::VBO input;

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

            BuildMesh(input, atomics, vertices, colors, normals, indices, request);
        }
    }

    void BuildMesh(
        Engine::Graphics::VBO& input,
        Engine::Graphics::VBO& atomics,
        Engine::Graphics::VBO& vertices,
        Engine::Graphics::VBO& colors,
        Engine::Graphics::VBO& normals,
        Engine::Graphics::VBO& indices,
        const Request& request
    )
    {
        mPrivate.profiler.Reset();

        input.BufferData(request.chunk->data());

        uint32_t count[2] = { 0, 0 };
        atomics.BufferData(sizeof(GLuint) * 2, count, GL_DYNAMIC_DRAW);

        {
            std::unique_lock<std::mutex> lock{ mShared.programMutex };
            if (mShared.program)
            {
                BIND_PROGRAM_IN_SCOPE(mShared.program);
                mShared.program->Uniform3ui("uChunkSize", kChunkSize, kChunkHeight, kChunkSize);

                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input.GetBuffer());
                glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, atomics.GetBuffer());
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vertices.GetBuffer());
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, colors.GetBuffer());
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, normals.GetBuffer());
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, indices.GetBuffer());
                glDispatchCompute(kChunkSize / 8, kChunkHeight / 16, kChunkSize / 8);

                // make sure writing to the buffers has finished
                glMemoryBarrier(GL_ALL_BARRIER_BITS);
                CHECK_GL_ERRORS();
            }
        }

        atomics.Bind(VBOTarget::VertexData);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(count), count);

        static Engine::Graphics::VBO sVertices;
        static Engine::Graphics::VBO sColors;
        static Engine::Graphics::VBO sNormals;
        static Engine::Graphics::VBO sIndices;

        sVertices.CopyFrom(vertices, sizeof(glm::vec4) * count[0]);
        sColors.CopyFrom(colors, sizeof(glm::vec4) * count[0]);
        sNormals.CopyFrom(normals, sizeof(glm::vec4) * count[0]);
        sIndices.CopyFrom(indices, sizeof(GLuint) * 4 * count[1]);
        glFlush();

        request.component->renderType = GL_TRIANGLES;
        request.component->Set(
            std::move(sVertices),
            std::move(sColors),
            std::move(sNormals),
            std::move(sIndices),
            count[0], size_t(count[1]) * 4
        );

        DebugHelper::Instance().SetMetric("Mesh generation time", mPrivate.profiler.Elapsed());
        if (request.resultFunction)
        {
            request.resultFunction();
        }
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

        LoadShader();
    }

    void Update()
    {
        if (ImGui::Begin("Chunk Mesh Generator"))
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

        if (!mLastError.empty())
        {
            ImGui::Begin("Mesh error");
            ImGui::Text("%s", mLastError.c_str());
            ImGui::End();
        }
    }

private:
    PrivateData mPrivate;
    SharedData mShared;

    std::string mLastError;

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

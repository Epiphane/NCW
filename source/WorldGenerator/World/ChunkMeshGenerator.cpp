// By Thomas Steinke

#include <queue>
#include <mutex>

#include <Engine/Core/Context.h>
#include <Engine/Core/Timer.h>
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
        std::thread thread;

        Engine::Timer<10> timer;
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

        // Whether the thread should exit.
        bool exiting = false;

        // Whether the thread has exited.
        bool exited = false;
    };

public:
    Worker()
        // : mContext(Engine::Window::Instance())
    {
        mPrivate.thread = std::thread([this] {
            Engine::Context::Instance().Activate();
            Engine::Window::Instance().GetVAO().Bind();
            Run();
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

                request = std::move(mShared.requests.front());
                mShared.requests.pop_front();
            }

            mPrivate.timer.Reset();
            BuildMesh(request);
            mPrivate.timer.Elapsed();
            DebugHelper::Instance().SetMetric("Generation time", mPrivate.timer.Average());
        }
    }

    GLuint MakePoint(
        std::vector<ShadedMesh::Point>& vertices,
        std::vector<GLuint>& indices,
        const glm::vec3& position,
        const glm::vec3& color,
        const glm::vec3& normal)
    {
        GLuint ndx = GLuint(vertices.size());
        vertices.push_back(ShadedMesh::Point{position, color, normal, 1.0f});
        indices.push_back(ndx);
        return ndx;
    };

    void BuildMesh(const Request& request)
    {
        constexpr float d = 0.5f;
        constexpr glm::vec3 d___{-d,-d,-d};
        constexpr glm::vec3 d__1{-d,-d, d};
        constexpr glm::vec3 d_1_{-d, d,-d};
        constexpr glm::vec3 d_11{-d, d, d};
        constexpr glm::vec3 d1__{d,-d,-d};
        constexpr glm::vec3 d1_1{d,-d, d};
        constexpr glm::vec3 d11_{d, d,-d};
        constexpr glm::vec3 d111{d, d, d};

        constexpr glm::vec3 top   { 0,  1,  0};
        constexpr glm::vec3 bottom{ 0, -1,  0};
        constexpr glm::vec3 left  {-1,  0,  0};
        constexpr glm::vec3 right { 1,  0,  0};
        constexpr glm::vec3 front { 0,  0,  1};
        constexpr glm::vec3 back  { 0,  0, -1};

        std::vector<ShadedMesh::Point> vertices;
        std::vector<GLuint> indices;
        vertices.reserve(kChunkSize * kChunkSize * 24);
        indices.reserve(kChunkSize * kChunkSize * 24);
        bool occupied[3][3][3];

        // Iterate over every x, z pair in the chunk.
        for (int x = 0; x < kChunkSize; ++x)
        {
            for (int z = 0; z < kChunkSize; ++z)
            {
                const Block& topBlock = request.chunk->GetTop(uint32_t(x), uint32_t(z));
                int height = int(topBlock.y);
                glm::vec3 color = topBlock.color;

                int y = height;
                bool blockVisible = true;

                uint8_t side = Voxel::All;
                do
                {
                    glm::vec3 position{x, y, z};

                    // Compute which neighboring coordinates are occupied.
                    for (int dx = 0; dx <= 2; ++dx)
                    {
                        int nx = x + dx - 1;
                        for (int dz = 0; dz <= 2; ++dz)
                        {
                            int nz = z + dz - 1;

                            for (int dy = 0; dy <= 2; ++dy)
                            {
                                int ny = y + dy - 1;

                                if (nx < 0 || nx >= kChunkSize ||
                                    nz < 0 || nz >= kChunkSize ||
                                    ny < 0 || ny >= kChunkHeight)
                                {
                                    occupied[dx][dy][dz] = false;
                                }
                                else
                                {
                                    uint32_t ceiling = request.chunk->GetTop(uint32_t(nx), uint32_t(nz)).y;
                                    occupied[dx][dy][dz] = uint32_t(ny) <= ceiling;
                                }
                            }
                        }
                    }

                    // Figure out what sides of the voxel to draw.
                    side = Voxel::All;
                    if (occupied[0][1][1]) { side ^= Voxel::Left; }
                    if (occupied[1][0][1]) { side ^= Voxel::Bottom; }
                    if (occupied[1][1][0]) { side ^= Voxel::Back; }
                    if (occupied[1][1][2]) { side ^= Voxel::Front; }
                    if (occupied[1][2][1]) { side ^= Voxel::Top; }
                    if (occupied[2][1][1]) { side ^= Voxel::Right; }

                    blockVisible = (side != 0);

                    //
                    // Now, draw each face.
                    //

                    // Top
                    if (side & Voxel::Top)
                    {
                        MakePoint(vertices, indices, position + d111, color, top);
                        MakePoint(vertices, indices, position + d11_, color, top);
                        MakePoint(vertices, indices, position + d_1_, color, top);
                        MakePoint(vertices, indices, position + d_11, color, top);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Bottom
                    if (side & Voxel::Bottom)
                    {
                        MakePoint(vertices, indices, position + d1_1, color, bottom);
                        MakePoint(vertices, indices, position + d__1, color, bottom);
                        MakePoint(vertices, indices, position + d___, color, bottom);
                        MakePoint(vertices, indices, position + d1__, color, bottom);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Left (x = -1)
                    if (side & Voxel::Left)
                    {
                        MakePoint(vertices, indices, position + d_11, color, left);
                        MakePoint(vertices, indices, position + d_1_, color, left);
                        MakePoint(vertices, indices, position + d___, color, left);
                        MakePoint(vertices, indices, position + d__1, color, left);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Right (x = 1)
                    if (side & Voxel::Right)
                    {
                        MakePoint(vertices, indices, position + d111, color, right);
                        MakePoint(vertices, indices, position + d1_1, color, right);
                        MakePoint(vertices, indices, position + d1__, color, right);
                        MakePoint(vertices, indices, position + d11_, color, right);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Back (z = -1)
                    if (side & Voxel::Back)
                    {
                        MakePoint(vertices, indices, position + d11_, color, back);
                        MakePoint(vertices, indices, position + d1__, color, back);
                        MakePoint(vertices, indices, position + d___, color, back);
                        MakePoint(vertices, indices, position + d_1_, color, back);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Front (z = 1)
                    if (side & Voxel::Front)
                    {
                        MakePoint(vertices, indices, position + d111, color, front);
                        MakePoint(vertices, indices, position + d_11, color, front);
                        MakePoint(vertices, indices, position + d__1, color, front);
                        MakePoint(vertices, indices, position + d1_1, color, front);
                        indices.push_back(kPrimitiveRestart);
                    }

                    --y;
                } while (side != 0 && y >= 0);
            }
        }

        Engine::Graphics::VBO vbo(Engine::Graphics::VBO::Vertices);
        vbo.BufferData(vertices);

        size_t count = indices.size();
        Engine::Graphics::VBO ndx(Engine::Graphics::VBO::Indices);
        ndx.BufferData(indices);
        glFlush();

        request.component->renderType = GL_TRIANGLE_FAN;
        request.component->Set(std::move(vbo), std::move(ndx), count);
        request.resultFunction();

    }

    void Add(const Request& request)
    {
        std::unique_lock<std::mutex> lock{mShared.mutex};
        mShared.requests.push_back(request);
        mShared.condition.notify_one();
    }

private:
    PrivateData mPrivate;
    SharedData mShared;

    //Engine::Context mContext;
};

///
///
///
ChunkMeshGenerator::ChunkMeshGenerator()
{
    mWorker.reset(new Worker());
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

}; // namespace CubeWorld

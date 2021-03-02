// By Thomas Steinke

#include <queue>
#include <mutex>

#include <Engine/Core/Context.h>
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

            BuildMesh(request);
        }
    }

    float ComputeOcclusion(glm::vec3 dPosition, glm::vec3 normal)
    {
        CUBEWORLD_UNREFERENCED_PARAMETER(dPosition);
        CUBEWORLD_UNREFERENCED_PARAMETER(normal);

        /*
        float blocked = 0;
        float total = 0;
        int dx = int(dposition.x / d) + 1;
        int dy = int(dposition.y / d) + 1;
        int dz = int(dposition.z / d) + 1;

        if (normal.x != 0)
        {
            for (int dy_ = dy - 1; dy_ <= dy + 1; ++dy_)
            {
                if (dy_ < 0 || dy_ > 2) continue;
                for (int dz_ = dz - 1; dz_ <= dz + 1; ++dz_)
                {
                    if (dz_ < 0 || dz_ > 2) continue;
                    total++;
                    blocked += occupied[int(normal.x) + 1][dy_][dz_] ? 1 : 0;
                }
            }
        }
        else if (normal.y != 0)
        {
            for (int dx_ = dx - 1; dx_ <= dx + 1; ++dx_)
            {
                if (dx_ < 0 || dx_ > 2) continue;
                for (int dz_ = dz - 1; dz_ <= dz + 1; ++dz_)
                {
                    if (dz_ < 0 || dz_ > 2) continue;
                    total++;
                    blocked += occupied[dx_][int(normal.y) + 1][dz_] ? 1 : 0;
                }
            }
        }
        else if (normal.z != 0)
        {
            for (int dx_ = dx - 1; dx_ <= dx + 1; ++dx_)
            {
                if (dx_ < 0 || dx_ > 2) continue;
                for (int dy_ = dy - 1; dy_ <= dy + 1; ++dy_)
                {
                    if (dy_ < 0 || dy_ > 2) continue;
                    total++;
                    blocked += occupied[dx_][dy_][int(normal.z) + 1] ? 1 : 0;
                }
            }
        }

        if (normal.y == 0)
        {
            blocked /= 2;
        }
        */

        return 1.0f;
    }

    GLuint MakePoint(
        std::vector<ShadedMesh::Point>& vertices,
        std::vector<GLuint>& indices,
        const glm::vec3& position,
        const glm::vec3& color,
        const glm::vec3& normal,
        float occlusion)
    {
        GLuint ndx = GLuint(vertices.size());
        vertices.push_back(ShadedMesh::Point{position, color, normal, occlusion});
        indices.push_back(ndx);
        return ndx;
    };

    void BuildMesh(const Request& request)
    {
        constexpr float d = 0.5f;
        constexpr glm::vec3 d___{-d,-d,-d};
        const glm::vec3 d__0{-d,-d, 0};
        const glm::vec3 d__1{-d,-d, d};
        const glm::vec3 d_0_{-d, 0,-d};
        const glm::vec3 d_00{-d, 0, 0};
        const glm::vec3 d_01{-d, 0, d};
        const glm::vec3 d_1_{-d, d,-d};
        const glm::vec3 d_10{-d, d, 0};
        const glm::vec3 d_11{-d, d, d};

        const glm::vec3 d0__{0,-d,-d};
        const glm::vec3 d0_0{0,-d, 0};
        const glm::vec3 d0_1{0,-d, d};
        const glm::vec3 d00_{0, 0,-d};
        const glm::vec3 d000{0, 0, 0};
        const glm::vec3 d001{0, 0, d};
        const glm::vec3 d01_{0, d,-d};
        const glm::vec3 d010{0, d, 0};
        const glm::vec3 d011{0, d, d};

        const glm::vec3 d1__{d,-d,-d};
        const glm::vec3 d1_0{d,-d, 0};
        const glm::vec3 d1_1{d,-d, d};
        const glm::vec3 d10_{d, 0,-d};
        const glm::vec3 d100{d, 0, 0};
        const glm::vec3 d101{d, 0, d};
        const glm::vec3 d11_{d, d,-d};
        const glm::vec3 d110{d, d, 0};
        const glm::vec3 d111{d, d, d};

        const glm::vec3 top{0,  1,  0};
        const glm::vec3 bottom{0, -1,  0};
        const glm::vec3 left{-1,  0,  0};
        const glm::vec3 right{1,  0,  0};
        const glm::vec3 front{0,  0,  1};
        const glm::vec3 back{0,  0, -1};

        std::vector<ShadedMesh::Point> vertices;
        std::vector<GLuint> indices;
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
                        float o111 = ComputeOcclusion(d111, top);
                        float o11_ = ComputeOcclusion(d11_, top);
                        float o_1_ = ComputeOcclusion(d_1_, top);
                        float o_11 = ComputeOcclusion(d_11, top);
                        float o110 = (o111 + o11_) / 2.0f;
                        float o011 = (o111 + o_11) / 2.0f;
                        float o_10 = (o_11 + o_1_) / 2.0f;
                        float o01_ = (o11_ + o_1_) / 2.0f;

                        MakePoint(vertices, indices, position + d010, color, top, (o111 + o11_ + o_1_ + o_11) / 4.0f);
                        GLuint begin = MakePoint(vertices, indices, position + d111, color, top, o111);
                        MakePoint(vertices, indices, position + d110, color, top, o110);
                        MakePoint(vertices, indices, position + d11_, color, top, o11_);
                        MakePoint(vertices, indices, position + d01_, color, top, o01_);
                        MakePoint(vertices, indices, position + d_1_, color, top, o_1_);
                        MakePoint(vertices, indices, position + d_10, color, top, o_10);
                        MakePoint(vertices, indices, position + d_11, color, top, o_11);
                        MakePoint(vertices, indices, position + d011, color, top, o011);
                        indices.push_back(begin);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Bottom
                    if (side & Voxel::Bottom)
                    {
                        float o1_1 = ComputeOcclusion(d1_1, bottom);
                        float o1__ = ComputeOcclusion(d1__, bottom);
                        float o___ = ComputeOcclusion(d___, bottom);
                        float o__1 = ComputeOcclusion(d__1, bottom);
                        float o1_0 = (o1_1 + o1__) / 2.0f;
                        float o0_1 = (o1_1 + o__1) / 2.0f;
                        float o__0 = (o__1 + o___) / 2.0f;
                        float o0__ = (o1__ + o___) / 2.0f;

                        MakePoint(vertices, indices, position + d0_0, color, bottom, (o1_1 + o1__ + o___ + o__1) / 4.0f);
                        GLuint begin = MakePoint(vertices, indices, position + d1_1, color, bottom, o1_1);
                        MakePoint(vertices, indices, position + d0_1, color, bottom, o0_1);
                        MakePoint(vertices, indices, position + d__1, color, bottom, o__1);
                        MakePoint(vertices, indices, position + d__0, color, bottom, o__0);
                        MakePoint(vertices, indices, position + d___, color, bottom, o___);
                        MakePoint(vertices, indices, position + d0__, color, bottom, o0__);
                        MakePoint(vertices, indices, position + d1__, color, bottom, o1__);
                        MakePoint(vertices, indices, position + d1_0, color, bottom, o1_0);
                        indices.push_back(begin);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Left (x = -1)
                    if (side & Voxel::Left)
                    {
                        float o_11 = ComputeOcclusion(d_11, left);
                        float o_1_ = ComputeOcclusion(d_1_, left);
                        float o___ = ComputeOcclusion(d___, left);
                        float o__1 = ComputeOcclusion(d__1, left);
                        float o_10 = (o_11 + o_1_) / 2.0f;
                        float o_01 = (o_11 + o__1) / 2.0f;
                        float o__0 = (o__1 + o___) / 2.0f;
                        float o_0_ = (o_1_ + o___) / 2.0f;

                        MakePoint(vertices, indices, position + d_00, color, left, (o_11 + o_1_ + o___ + o__1) / 4.0f);
                        GLuint begin = MakePoint(vertices, indices, position + d_11, color, left, o_11);
                        MakePoint(vertices, indices, position + d_10, color, left, o_10);
                        MakePoint(vertices, indices, position + d_1_, color, left, o_1_);
                        MakePoint(vertices, indices, position + d_0_, color, left, o_0_);
                        MakePoint(vertices, indices, position + d___, color, left, o___);
                        MakePoint(vertices, indices, position + d__0, color, left, o__0);
                        MakePoint(vertices, indices, position + d__1, color, left, o__1);
                        MakePoint(vertices, indices, position + d_01, color, left, o_01);
                        indices.push_back(begin);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Right (x = 1)
                    if (side & Voxel::Right)
                    {
                        float o111 = ComputeOcclusion(d111, right);
                        float o11_ = ComputeOcclusion(d11_, right);
                        float o1__ = ComputeOcclusion(d1__, right);
                        float o1_1 = ComputeOcclusion(d1_1, right);

                        float o110 = (o111 + o11_) / 2.0f;
                        float o101 = (o111 + o1_1) / 2.0f;
                        float o1_0 = (o1_1 + o1__) / 2.0f;
                        float o10_ = (o11_ + o1__) / 2.0f;

                        MakePoint(vertices, indices, position + d100, color, right, (o111 + o11_ + o1__ + o1_1) / 4.0f);
                        GLuint begin = MakePoint(vertices, indices, position + d111, color, right, o111);
                        MakePoint(vertices, indices, position + d101, color, right, o101);
                        MakePoint(vertices, indices, position + d1_1, color, right, o1_1);
                        MakePoint(vertices, indices, position + d1_0, color, right, o1_0);
                        MakePoint(vertices, indices, position + d1__, color, right, o1__);
                        MakePoint(vertices, indices, position + d10_, color, right, o10_);
                        MakePoint(vertices, indices, position + d11_, color, right, o11_);
                        MakePoint(vertices, indices, position + d110, color, right, o110);
                        indices.push_back(begin);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Back (z = -1)
                    if (side & Voxel::Back)
                    {
                        float o1__ = ComputeOcclusion(d1__, back);
                        float o11_ = ComputeOcclusion(d11_, back);
                        float o___ = ComputeOcclusion(d___, back);
                        float o_1_ = ComputeOcclusion(d_1_, back);
                        float o10_ = (o11_ + o1__) / 2.0f;
                        float o0__ = (o1__ + o___) / 2.0f;
                        float o_0_ = (o___ + o_1_) / 2.0f;
                        float o01_ = (o_1_ + o11_) / 2.0f;


                        MakePoint(vertices, indices, position + d00_, color, back, (o___ + o11_ + o1__ + o_1_) / 4.0f);
                        GLuint begin = MakePoint(vertices, indices, position + d11_, color, back, o11_);
                        MakePoint(vertices, indices, position + d10_, color, back, o10_);
                        MakePoint(vertices, indices, position + d1__, color, back, o1__);
                        MakePoint(vertices, indices, position + d0__, color, back, o0__);
                        MakePoint(vertices, indices, position + d___, color, back, o___);
                        MakePoint(vertices, indices, position + d_0_, color, back, o_0_);
                        MakePoint(vertices, indices, position + d_1_, color, back, o_1_);
                        MakePoint(vertices, indices, position + d01_, color, back, o01_);
                        indices.push_back(begin);
                        indices.push_back(kPrimitiveRestart);
                    }

                    // Front (z = 1)
                    if (side & Voxel::Front)
                    {
                        float o1_1 = ComputeOcclusion(d1_1, front);
                        float o111 = ComputeOcclusion(d111, front);
                        float o__1 = ComputeOcclusion(d__1, front);
                        float o_11 = ComputeOcclusion(d_11, front);
                        float o101 = (o111 + o1_1) / 2.0f;
                        float o0_1 = (o1_1 + o__1) / 2.0f;
                        float o_01 = (o__1 + o_11) / 2.0f;
                        float o011 = (o_11 + o111) / 2.0f;

                        MakePoint(vertices, indices, position + d001, color, front, (o__1 + o111 + o1_1 + o_11) / 4.0f);
                        GLuint begin = MakePoint(vertices, indices, position + d111, color, front, o111);
                        MakePoint(vertices, indices, position + d011, color, front, o011);
                        MakePoint(vertices, indices, position + d_11, color, front, o_11);
                        MakePoint(vertices, indices, position + d_01, color, front, o_01);
                        MakePoint(vertices, indices, position + d__1, color, front, o__1);
                        MakePoint(vertices, indices, position + d0_1, color, front, o0_1);
                        MakePoint(vertices, indices, position + d1_1, color, front, o1_1);
                        MakePoint(vertices, indices, position + d101, color, front, o101);
                        indices.push_back(begin);
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

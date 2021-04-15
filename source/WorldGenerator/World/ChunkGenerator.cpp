// By Thomas Steinke

#include <queue>
#include <mutex>

#include <RGBLogger/Logger.h>

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
    {
        gHeightmodule.SetFrequency(0.25f);

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

                request = std::move(mShared.requests.front());
                mShared.requests.pop_front();
            }

            BuildChunk(request);
        }
    }

    void BuildChunk(const Request& request)
    {
        noise::utils::NoiseMap map;
        noise::utils::NoiseMapBuilderPlane builder;

        int chunkX = request.coordinates.x;
        int chunkZ = request.coordinates.z;

        builder.SetSourceModule(gHeightmodule);
        builder.SetDestNoiseMap(map);
        builder.SetDestSize(kChunkSize, kChunkSize);
        builder.SetBounds(chunkX, chunkX + 1, chunkZ, chunkZ + 1);
        builder.Build();

        const glm::vec4 DEEP = glm::vec4(0, 0, 128, 255) / 255.f;
        const glm::vec4 SHALLOW = glm::vec4(0, 0, 255, 255) / 255.f;
        const glm::vec4 SHORE = glm::vec4(0, 128, 255, 255) / 255.f;
        const glm::vec4 SAND = glm::vec4(240, 240, 64, 255) / 255.f;
        const glm::vec4 GRASS = glm::vec4(32, 160, 0, 255) / 255.f;
        const glm::vec4 DIRT = glm::vec4(224, 224, 0, 255) / 255.f;
        const glm::vec4 ROCK = glm::vec4(128, 128, 128, 255) / 255.f;
        const glm::vec4 SNOW = glm::vec4(255, 255, 255, 255) / 255.f;

        Chunk chunk(request.coordinates);
        for (uint32_t x = 0; x < kChunkSize; ++x)
        {
            for (uint32_t z = 0; z < kChunkSize; ++z)
            {
                // Elevation is between [-1, 1]
                float elevation = map.GetValue((int)x, (int)z);
                elevation = std::clamp(elevation - 0.25f, -0.75f, 1.f);

                uint32_t height = uint32_t((elevation + 1) * kChunkHeight / 4);

                glm::vec4 source, dest;
                float start, end;
                if (elevation >= 0.75f) { source = ROCK; dest = SNOW; start = 0.75f; end = 1.0f; }
                else if (elevation >= 0.375f) { source = DIRT; dest = ROCK; start = 0.375f; end = 0.75f; }
                else if (elevation >= 0.125f) { source = GRASS; dest = DIRT; start = 0.125f; end = 0.375f; }
                else if (elevation >= 0.0225f) { source = SAND; dest = GRASS; start = 0.0625f; end = 0.125f; }
                else if (elevation >= -0.25f) { source = SHORE; dest = SAND; start = 0; end = 0.0625f; }
                else if (elevation >= -0.5f) { source = SHALLOW; dest = SHORE; start = -0.25f; end = 0; }
                else { source = DEEP; dest = SHALLOW; start = -1.0f; end = -0.25f; }
                float perc = (elevation - start) / (end - start);

                glm::vec4 color = dest * perc + source * (1 - perc);

                Block& block = chunk.Get(x, height, z);
                block.color = color;
            }
        }

        request.resultFunction(std::move(chunk));
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
};

///
///
///
ChunkGenerator::ChunkGenerator()
{
    mWorker.reset(new Worker());
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

}; // namespace CubeWorld

// By Thomas Steinke

#include <queue>
#include <mutex>

#include <RGBDesignPatterns/Macros.h>
#include <RGBLogger/Logger.h>
#include <Engine/Core/Timer.h>
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
        std::thread thread;

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

        // Whether the thread should exit.
        bool exiting = false;

        // Whether the thread has exited.
        bool exited = false;
    };

public:
    Worker()
    {
        gHeightmodule.SetFrequency(0.01f);
        gHeightmodule.SetOctaveCount(8);
        gHeightmodule.SetSeed(5);

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
        mPrivate.profiler.Reset();

        int chunkX = request.coordinates.x + 100;// +100;
        int chunkY = request.coordinates.y;// +100;
        int chunkZ = request.coordinates.z + 100;// + 100;

        Chunk chunk(request.coordinates);
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

///
///
///
void ChunkGenerator::Clear()
{
    mWorker->ClearQueue();
}

}; // namespace CubeWorld

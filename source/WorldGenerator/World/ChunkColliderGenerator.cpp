// By Thomas Steinke

#include <queue>
#include <mutex>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <RGBDesignPatterns/Macros.h>
#include <Engine/Core/Context.h>
#include <Engine/Core/Timer.h>

#include "ChunkColliderGenerator.h"

namespace CubeWorld
{

struct ChunkColliderGenerator::RequestQueue
{
    // Protects the queue of requests.
    std::mutex mutex;

    // Queued requests for generation.
    std::deque<Request> requests;

    // Condition variable which signals that a new request was queued.
    std::condition_variable condition;
};

class ChunkColliderGenerator::Worker
{
public:
    //
    // Data owned by this specific thread.
    //
    struct PrivateData
    {
        // Worker thread.
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
        mPrivate.thread = std::thread([this] { Run(); });
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

            ComputeHeights(request);
        }
    }

    void ComputeHeights(const Request& request)
    {
        const Chunk& chunk = *request.chunk;
        std::vector<short> heights;
        heights.resize((kChunkSize + 1) * (kChunkSize + 1));

        // Keep track of the "last height", for easier testing of the next one
        uint16_t cursor = 0;
        short* pHeight = heights.data();
        for (uint32_t j = 0; j < kChunkSize; ++j)
        {
            cursor = 0;
            for (uint32_t i = 0; i < kChunkSize; ++i)
            {
                // Move upwards until we find empty space.
                bool isBlock = chunk.Get(i, cursor, j).color.a > 0;
                while (isBlock && cursor < kChunkHeight - 1)
                {
                    isBlock = chunk.Get(i, ++cursor, j).color.a > 0;
                }

                // Move downwards until there is a block directly below.
                bool isBlockBelow = cursor == 0 ? true : chunk.Get(i, uint32_t(cursor - 1), j).color.a > 0;
                while (cursor > 1 && !isBlockBelow)
                {
                    isBlockBelow = chunk.Get(i, uint32_t(--cursor - 1), j).color.a > 0;
                }

                assert(!isBlock && isBlockBelow);

                // Move to the next element
                *pHeight++ = short(cursor);
            }

            // Add a new element at the end that's just the same high as its predecessor
            *pHeight++ = short(cursor);
        }

        for (uint32_t i = 0; i < kChunkSize + 1; ++i)
        {
            *pHeight++ = short(cursor);
        }

        request.resultFunction(std::move(heights));
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
    {}

private:
    PrivateData mPrivate;
    SharedData mShared;
};

///
///
///
ChunkColliderGenerator::ChunkColliderGenerator(Engine::EventManager&)
{
    mWorker.reset(new Worker());
}

///
///
///
ChunkColliderGenerator::~ChunkColliderGenerator()
{
}

///
///
///
void ChunkColliderGenerator::Add(const Request& request)
{
    mWorker->Add(request);
}

///
///
///
void ChunkColliderGenerator::Clear()
{
    mWorker->ClearQueue();
}

///
///
///
void ChunkColliderGenerator::Update()
{
    mWorker->Update();
}

}; // namespace CubeWorld

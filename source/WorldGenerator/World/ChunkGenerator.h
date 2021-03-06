// By Thomas Steinke

#pragma once

#include <memory>
#include <functional>
#include <thread>

#include "Chunk.h"

namespace CubeWorld
{

class ChunkGenerator
{
public:
    struct Request
    {
        // The chunk's location.
        ChunkCoords coordinates = ChunkCoords{ 0, 0, 0 };

        // Function to be called with the finished result.
        std::function<void(std::unique_ptr<Chunk>&&)> resultFunction;
    };

public:
    ChunkGenerator(Engine::EventManager& events);
    ~ChunkGenerator();

    void Add(const Request& request);
    void Clear();

    void Update();

private:
    struct RequestQueue;
    class Worker;

    // Worker thread.
    std::unique_ptr<Worker> mWorker;
};

}; // namespace CubeWorld

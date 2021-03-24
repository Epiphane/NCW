// By Thomas Steinke

#pragma once

#include <memory>
#include <functional>
#include <thread>

#include <Shared/Helpers/Noise.h>

#include "Chunk.h"

namespace CubeWorld
{

class ChunkGenerator
{
public:
    struct Request
    {
        // The chunk's location.
        ChunkCoords coordinates;

        // Function to be called with the finished result.
        std::function<void(Chunk&&)> resultFunction;
    };

public:
    ChunkGenerator();
    ~ChunkGenerator();

    void Add(const Request& request);

private:
    struct RequestQueue;
    class Worker;

    noise::module::Perlin mHeightmodule;

    std::unique_ptr<Worker> mWorker;
};

}; // namespace CubeWorld

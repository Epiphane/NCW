// By Thomas Steinke

#pragma once

#include <memory>
#include <functional>
#include <thread>

#include <Shared/Systems/BulletPhysicsSystem.h>

#include "Chunk.h"

namespace CubeWorld
{

class ChunkColliderGenerator
{
public:
    struct Request
    {
        // The chunk to generate a mesh for.
        Chunk* chunk = nullptr;

        // Function to be called with the finished result.
        std::function<void(std::vector<short>&&)> resultFunction;
    };

public:
    ChunkColliderGenerator(Engine::EventManager& events);
    ~ChunkColliderGenerator();

    void Add(const Request& request);
    void Clear();

    void Update();

private:
    struct RequestQueue;
    class Worker;

    std::unique_ptr<Worker> mWorker;
};

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <memory>
#include <functional>
#include <thread>

#include <Engine/Graphics/VBO.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

#include "Chunk.h"

namespace CubeWorld
{

class ChunkMeshGenerator
{
public:
    struct Request
    {
        // The chunk to generate a mesh for.
        Chunk* chunk = nullptr;

        // Component to buffer the data to.
        Engine::ComponentHandle<ShadedMesh> component;

        // Function to be called with the finished result.
        std::function<void()> resultFunction;
    };

public:
    ChunkMeshGenerator();
    ~ChunkMeshGenerator();

    void Add(const Request& request);
    void Clear();

    void Update();

private:
    struct RequestQueue;
    class Worker;

    std::unique_ptr<Worker> mWorker;
};

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <mutex>
#include <unordered_map>
#include <tuple>
#include <vector>

#include <Shared/Helpers/Noise.h>

#include "Chunk.h"
#include "ChunkGenerator.h"
#include "ChunkMeshGenerator.h"

namespace std
{

template <>
struct hash<CubeWorld::ChunkCoords>
{
    std::size_t operator()(const CubeWorld::ChunkCoords& k) const
    {
        return ((hash<int>()(k.x) ^ (hash<int>()(k.y) << 1)) >> 1) ^ (hash<int>()(k.z) << 1);
    }
};

}; // namespace std

namespace CubeWorld
{

class World
{
public:
    World();
    ~World();

    void Build();
    Engine::Entity Create(int chunkX, int chunkY, int chunkZ, Engine::EntityManager& entities);
    Chunk& Get(const ChunkCoords& coords);

private:
    void OnChunkGenerated(Chunk&& chunk);

private:
    noise::module::Perlin mHeightmodule;
    noise::utils::NoiseMap mHeightmap;

    // TODO maybe one day, we won't be able to keep a big ol' list of chunks here.
    // Until then, stay lazy.
    std::mutex mEntitiesMutex;
    std::unordered_map<ChunkCoords, Engine::Entity> mEntities;

    std::mutex mChunksMutex;
    std::unordered_map<ChunkCoords, Chunk> mChunks;

    std::unique_ptr<ChunkGenerator> mChunkGenerator;
    std::unique_ptr<ChunkMeshGenerator> mChunkMeshGenerator;
};

}; // namespace CubeWorld

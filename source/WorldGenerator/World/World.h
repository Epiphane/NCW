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
    World(Engine::EntityManager& entities, Engine::EventManager& events);
    ~World();

    void Build();
    void Reset();
    Engine::Entity Create(int chunkX, int chunkY, int chunkZ);
    Chunk& Get(const ChunkCoords& coords);

    void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);

private:
    void OnChunkGenerated(int version, Chunk&& chunk);

private:
    bool mQuitting = false;

    std::mutex mVersionMutex;
    int mVersion = 0;

    Engine::EntityManager& mEntityManager;
    Engine::EventManager& mEventManager;

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

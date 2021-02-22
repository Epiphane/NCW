// By Thomas Steinke

#pragma once

#include <memory>
#include <unordered_map>
#include <tuple>
#include <vector>

#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>
#include <Shared/Helpers/Noise.h>

namespace CubeWorld
{

struct ChunkCoords
{
    int x, y, z;

    bool operator==(const ChunkCoords& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
};

}; // namespace CubeWorld

namespace std
{

template <>
struct hash<CubeWorld::ChunkCoords>
{
    std::size_t operator()(const CubeWorld::ChunkCoords& k) const
    {
        using std::hash;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return ((hash<int>()(k.x)
            ^ (hash<int>()(k.y) << 1)) >> 1)
            ^ (hash<int>()(k.z) << 1);
    }
};

}; // namespace std

namespace CubeWorld
{

///
///
///
constexpr int kChunkSize = 64;
constexpr int kChunkHeight = 64;

struct Block
{
    bool isEmpty = true;
};

class Chunk
{
public:
    Chunk(const ChunkCoords& coord);
    ~Chunk();

    int GetHeight(int x, int z);

    ChunkCoords GetCoords() const { return mCoords; }
    bool IsPopulated() const { return mIsPopulated; }
    Block& Get(int x, int y, int z);

private:
    ChunkCoords mCoords;
    bool mIsPopulated = false;
    std::unique_ptr<Block> blocks;
};

class World
{
public:
    World();
    ~World();

    void Build();
    Engine::Entity Create(int chunkX, int chunkY, int chunkZ, Engine::EntityManager& entities);
    Chunk& Get(const ChunkCoords& coords);

private:
    noise::module::Perlin mHeightmodule;
    noise::utils::NoiseMap mHeightmap;
    noise::utils::NoiseMapBuilderPlane mBuilder;

    // TODO maybe one day, we won't be able to keep a big ol' list of chunks here.
    // Until then, stay lazy.
    std::unordered_map<ChunkCoords, Chunk> mChunks;
};

}; // namespace CubeWorld

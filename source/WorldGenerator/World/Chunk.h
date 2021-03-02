// By Thomas Steinke

#pragma once

#include <mutex>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

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

///
///
///
constexpr int kChunkSize = 64;
constexpr int kChunkHeight = 64;

struct Block
{
    uint32_t y;
    glm::vec3 color;
};

class Chunk
{
public:
    Chunk(const ChunkCoords& coord);
    Chunk(Chunk&& other);
    ~Chunk();

    const Block& GetTop(uint32_t x, uint32_t z) const;

    ChunkCoords GetCoords() const { return mCoords; }
    bool IsPopulated() const { return mIsPopulated; }
    bool Has(uint32_t x, uint32_t y, uint32_t z) const;
    Block& Get(uint32_t x, uint32_t y, uint32_t z);

private:
    ChunkCoords mCoords;
    bool mIsPopulated = false;

    mutable std::mutex mBlocksMutex;
    std::vector<std::vector<Block>> mBlocks;
};

}; // namespace CubeWorld

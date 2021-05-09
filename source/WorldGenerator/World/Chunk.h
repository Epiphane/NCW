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
constexpr int kChunkSize = 16;
constexpr int kChunkHeight = 48;

struct Block
{
    uint8_t r = 4;
    uint8_t g = 5;
    uint8_t b = 6;
    uint8_t a = 7;
};

class Chunk
{
public:
    Chunk(const ChunkCoords& coord);
    Chunk(Chunk&& other) noexcept;
    ~Chunk();

    ChunkCoords GetCoords() const { return mCoords; }
    bool IsPopulated() const { return mIsPopulated; }
    Block& Get(uint32_t x, uint32_t y, uint32_t z);

private:
    ChunkCoords mCoords;
    bool mIsPopulated = false;

    mutable std::mutex mBlocksMutex;
    std::vector<Block> mBlocks;
};

}; // namespace CubeWorld

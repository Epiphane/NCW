// By Thomas Steinke

#include <algorithm>
#include <RGBLogger/Logger.h>

#include "Chunk.h"

namespace CubeWorld
{

///
///
///
Chunk::Chunk(const ChunkCoords& coords)
    : mCoords(coords)
{
    mBlocks.resize(kChunkSize * kChunkSize * kChunkHeight);
}

///
///
///
Chunk::Chunk(Chunk&& other) noexcept
    : mCoords(other.mCoords)
    , mIsPopulated(other.mIsPopulated)
    , mBlocks(std::move(other.mBlocks))
{
}

///
///
///
Chunk::~Chunk()
{}

///
///
///
Block& Chunk::Get(uint32_t x, uint32_t y, uint32_t z)
{
    std::unique_lock<std::mutex> lock{mBlocksMutex};

    size_t index = x + z * kChunkSize + y * kChunkSize * kChunkSize;
    return mBlocks[index];
}

}; // namespace CubeWorld

// By Thomas Steinke

#include <algorithm>

#include "Chunk.h"

namespace CubeWorld
{

///
///
///
Chunk::Chunk(const ChunkCoords& coords)
    : mCoords(coords)
{
    mBlocks.resize(kChunkSize * kChunkSize);
}

///
///
///
Chunk::Chunk(Chunk&& other)
    : mCoords(other.mCoords)
    , mIsPopulated(other.mIsPopulated)
    , mBlocks(std::move(other.mBlocks))
{}

///
///
///
Chunk::~Chunk()
{}

///
///
///
const Block& Chunk::GetTop(uint32_t x, uint32_t z) const
{
    std::unique_lock<std::mutex> lock{mBlocksMutex};

    size_t index = x + z * kChunkSize;
    const std::vector<Block>& cell = mBlocks[index];
    assert(cell.size() != 0);

    auto it = std::max_element(cell.begin(), cell.end(), [](const Block& a, const Block& b) { return a.y < b.y; });
    assert(it != cell.end());
    return *it;
}

///
///
///
bool Chunk::Has(uint32_t x, uint32_t y, uint32_t z) const
{
    std::unique_lock<std::mutex> lock{mBlocksMutex};

    size_t index = x + z * kChunkSize;
    const std::vector<Block>& cell = mBlocks[index];
    auto it = std::find_if(cell.begin(), cell.end(), [y](const Block& b) { return b.y == y; });
    return it != cell.end();
}

///
///
///
Block& Chunk::Get(uint32_t x, uint32_t y, uint32_t z)
{
    std::unique_lock<std::mutex> lock{mBlocksMutex};

    size_t index = x + z * kChunkSize;
    std::vector<Block>& cell = mBlocks[index];
    auto it = std::find_if(cell.begin(), cell.end(), [y](const Block& b) { return b.y == y; });
    if (it != cell.end())
    {
        return *it;
    }

    Block b;
    b.y = y;
    cell.push_back(std::move(b));
    return cell.back();
}

}; // namespace CubeWorld

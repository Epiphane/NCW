// By Thomas Steinke

#include <cassert>
#include <functional>
#include <queue>

#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <Engine/Entity/Transform.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/Components/VoxModel.h>
#include <Shared/Systems/AnimationSystem.h>
#include <Shared/Systems/AnimationEventSystem.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/CombatSystem.h>
#include <Shared/Systems/FollowerSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/BulletPhysicsDebug.h>
#include <Shared/Systems/BulletPhysicsSystem.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/SimpleParticleSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>
#include <Shared/Systems/WalkSystem.h>
#include <Shared/Systems/WalkAnimationSystem.h>

#include <RGBDesignPatterns/Macros.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "World.h"

namespace CubeWorld
{

///
///
///
Chunk::Chunk(const ChunkCoords& coords)
    : mCoords(coords)
    , blocks(new Block[kChunkSize * kChunkSize * kChunkHeight])
{}

///
///
///
Chunk::~Chunk()
{}

///
///
///
int Chunk::GetHeight(int x, int z)
{
    for (int y = kChunkHeight - 1; y > 0; y--)
    {
        if (!Get(x, y, z).isEmpty)
        {
            return y;
        }
    }

    return 0;
}

///
///
///
Block& Chunk::Get(int x, int y, int z)
{
    int index = int((x + z * kChunkSize) * kChunkSize + y);
    return blocks.get()[index];
}

///
///
///
World::World()
{}

///
///
///
World::~World()
{}

///
///
///
void World::Build()
{
    mBuilder.SetSourceModule(mHeightmodule);
    mBuilder.SetDestNoiseMap(mHeightmap);
    mBuilder.SetDestSize(kChunkSize, kChunkSize);
    mBuilder.SetBounds(6, 10, 1, 5);
    mBuilder.Build();
}

///
///
///
Engine::Entity World::Create(int chunkX, int chunkY, int chunkZ, Engine::EntityManager& entities)
{
    noise::utils::NoiseMap map;
    noise::utils::NoiseMapBuilderPlane builder;

    builder.SetSourceModule(mHeightmodule);
    builder.SetDestNoiseMap(map);
    builder.SetDestSize(kChunkSize, kChunkSize);
    builder.SetBounds(chunkX, chunkX + 1, chunkZ, chunkZ + 1);
    builder.Build();

    Engine::Entity entity = entities.Create(
        float(chunkX) * kChunkSize,
        kChunkHeight / -2,
        float(chunkZ) * kChunkSize
    );

    Chunk& chunk = Get({chunkX, chunkY, chunkZ});
    std::vector<Voxel::Data> carpet;
    for (int x = 0; x < kChunkSize; ++x)
    {
        for (int z = 0; z < kChunkSize; ++z)
        {
            float elevation = 2 * map.GetValue(x, z);
            int height = int(std::floor((elevation + 1) * kChunkHeight / 8));

            for (int i = 0; i < height; ++i)
            {
                chunk.Get(x, i, z).isEmpty = false;
            }
        }
    }

    glm::vec4 DEEP(0, 0, 128, 1);
    glm::vec4 SHALLOW(0, 0, 255, 1);
    glm::vec4 SHORE(0, 128, 255, 1);
    glm::vec4 SAND(240, 240, 64, 1);
    glm::vec4 GRASS(32, 160, 0, 1);
    glm::vec4 DIRT(224, 224, 0, 1);
    glm::vec4 ROCK(128, 128, 128, 1);
    glm::vec4 SNOW(255, 255, 255, 1);

    for (int x = 0; x < kChunkSize; ++x)
    {
        for (int z = 0; z < kChunkSize; ++z)
        {
            int height = chunk.GetHeight(x, z);

            for (int dy = -1; dy <= 0; ++dy)
            {

                float elevation = float(height + dy) * 3 / kChunkHeight - 0.1f;

                //float elevation = 0.5f + 2 * float(map.GetValue(x, z));
                //elevation = std::floor(elevation * 30 + float(dy) * kChunkHeight / 8) / 30.0f;
                glm::vec4 source, dest;
                float start, end;
                if (elevation >= 0.75f) { source = ROCK; dest = SNOW; start = 0.75f; end = 1.0f; }
                else if (elevation >= 0.375f) { source = DIRT; dest = ROCK; start = 0.375f; end = 0.75f; }
                else if (elevation >= 0.125f) { source = GRASS; dest = DIRT; start = 0.125f; end = 0.375f; }
                else if (elevation >= 0.0625f) { source = SAND; dest = GRASS; start = 0.0625f; end = 0.125f; }
                else if (elevation >= 0.0f) { source = SHORE; dest = SAND; start = 0; end = 0.0625f; }
                else if (elevation >= -0.25f) { source = SHALLOW; dest = SHORE; start = -0.25f; end = 0; }
                else { source = DEEP; dest = SHALLOW; start = -1.0f; end = -0.25f; }
                float perc = (elevation - start) / (end - start);

                glm::vec3 position = glm::vec3(x, height + dy, z);
                glm::vec4 color = glm::vec4(87, 239, 7, 1);
                color = dest * perc + source * (1 - perc);

                uint32_t occlusion = 0;
                if (x > 0)
                {
                    if (!chunk.Get(x - 1, height + 1, z).isEmpty)
                    {
                        occlusion |= 0x00770000;
                    }

                    if (z > 0 && !chunk.Get(x - 1, height + 1, z - 1).isEmpty)
                    {
                        occlusion |= 0x00700000;
                    }

                    if (z < kChunkSize - 1 && !chunk.Get(x - 1, height + 1, z + 1).isEmpty)
                    {
                        occlusion |= 0x00070000;
                    }

                    if (!chunk.Get(x - 1, height - 1, z).isEmpty)
                    {
                        occlusion |= 0x77000000;
                    }

                    if (z > 0 && !chunk.Get(x - 1, height - 1, z - 1).isEmpty)
                    {
                        occlusion |= 0x70000000;
                    }

                    if (z < kChunkSize - 1 && !chunk.Get(x - 1, height - 1, z + 1).isEmpty)
                    {
                        occlusion |= 0x07000000;
                    }
                }

                if (x < kChunkSize - 1)
                {
                    if (!chunk.Get(x + 1, height + 1, z).isEmpty)
                    {
                        occlusion |= 0x00000077;
                    }

                    if (z > 0 && !chunk.Get(x + 1, height + 1, z - 1).isEmpty)
                    {
                        occlusion |= 0x00000070;
                    }

                    if (z < kChunkSize - 1 && !chunk.Get(x + 1, height + 1, z + 1).isEmpty)
                    {
                        occlusion |= 0x00000007;
                    }

                    if (!chunk.Get(x + 1, height - 1, z).isEmpty)
                    {
                        occlusion |= 0x00007700;
                    }

                    if (z > 0 && !chunk.Get(x + 1, height - 1, z - 1).isEmpty)
                    {
                        occlusion |= 0x00007000;
                    }

                    if (z < kChunkSize - 1 && !chunk.Get(x + 1, height - 1, z + 1).isEmpty)
                    {
                        occlusion |= 0x00000700;
                    }
                }

                if (z > 0)
                {
                    if (!chunk.Get(x, height + 1, z - 1).isEmpty)
                    {
                        occlusion |= 0x00700070;
                    }

                    if (!chunk.Get(x, height - 1, z - 1).isEmpty)
                    {
                        occlusion |= 0x70007000;
                    }
                }

                if (z < kChunkSize - 1)
                {
                    if (!chunk.Get(x, height + 1, z + 1).isEmpty)
                    {
                        occlusion |= 0x00070007;
                    }

                    if (!chunk.Get(x, height + 1, z + 1).isEmpty)
                    {
                        occlusion |= 0x07000700;
                    }
                }

                occlusion &= 0x55ff55ff;

                carpet.push_back(Voxel::Data(position, color, Voxel::All, occlusion));
            }
        }
    }

    // Create mesh from carpet.
    // std::vector<GLfloat> points;
    // std::vector<GLfloat> colors;
    // entity.Add<Simple3DRender>(std::move(points), std::move(colors));

   entity.Add<VoxelRender>(std::move(carpet));

    /*
    auto makeCollider = [&](int i, int j, int height, int width, int length) {
        Engine::Entity collider = entities.Create(i - size + float(width - 1) / 2, float(height), j - size + float(length - 1) / 2);
        collider.Add<BulletPhysics::StaticBody>(glm::vec3(width, 1, length));

        for (int x = i; x < i + width; ++x)
        {
            for (int y = j; y < j + length; ++y)
            {
                assert(!used[index(x, y)]);
                if (heights[index(x, y)] == height) used[index(x, y)] = true;
            }
        }
    };

    for (int x = 0; x < kChunkSize; ++x)
    {
        for (int z = 0; z < kChunkSize; ++z)
        {
            int32_t height = chunk->GetHeight(x, z);

            // Attempt 3: Same as 2, but allow blocks to sit under each other
            // Result: Generated 933 blocks
            int width = 0, length = 0;
            int nextWidth = 1, nextLength = 1;
            while (nextWidth > width || nextLength > length)
            {
                width = nextWidth++;
                length = nextLength++;

                // Don't exceed the bounds of the chunk
                if (x + width >= kChunkSize)
                {
                    --nextWidth;
                }
                else
                {
                    for (int n = 0; n < length; ++n)
                    {
                        if (chunk->GetHeight(x + width,
                            used[index(i + nextWidth - 1, j + n)] ||
                            heights[index(i + nextWidth - 1, j + n)] < height
                            )
                        {
                            --nextWidth;
                            break;
                        }
                    }
                }

                // Don't exceed the bounds of the chunk
                if (z + length >= kChunkSize)
                {
                    --nextLength;
                }
                else
                {
                    for (int n = 0; n < nextWidth; ++n)
                    {
                        if (
                            used[index(i + n, j + nextLength - 1)] ||
                            heights[index(i + n, j + nextLength - 1)] < height
                            )
                        {
                            --nextLength;
                            break;
                        }
                    }
                }
            }

            makeCollider(i, j, height, width, length);
        }
    }
    */

    return entity;
}

///
///
///
Chunk& World::Get(const ChunkCoords& coords)
{
    if (mChunks.count(coords) == 0)
    {
        mChunks.emplace(coords, coords);
    }

    return mChunks.at(coords);
}

}; // namespace CubeWorld

// By Thomas Steinke

#include <algorithm>
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
World::World()
    : mChunkGenerator(new ChunkGenerator())
    , mChunkMeshGenerator(new ChunkMeshGenerator())
{
    mHeightmodule.SetFrequency(0.5);
    mHeightmodule.SetOctaveCount(8);
}

///
///
///
World::~World()
{}

///
///
///
void World::Build()
{}

///
///
///
Engine::Entity World::Create(int chunkX, int chunkY, int chunkZ, Engine::EntityManager& entities)
{
    ChunkCoords coordinates{chunkX, chunkY, chunkZ};

    ChunkGenerator::Request request;
    request.coordinates = coordinates;
    request.resultFunction = std::bind(&World::OnChunkGenerated, this, std::placeholders::_1);
    mChunkGenerator->Add(request);

    Engine::Entity entity = entities.Create(
        float(chunkX) * kChunkSize,
        kChunkHeight / -2,
        float(chunkZ) * kChunkSize
    );

    entity.Add<ShadedMesh>();

    std::unique_lock<std::mutex> lock{mEntitiesMutex};
    mEntities.emplace(coordinates, entity);

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
    std::unique_lock<std::mutex> lock{mChunksMutex};
    if (mChunks.count(coords) == 0)
    {
        mChunks.emplace(coords, coords);
    }

    return mChunks.at(coords);
}

/// 
/// 
/// 
void World::OnChunkGenerated(Chunk&& chunk)
{
    ChunkCoords coordinates = chunk.GetCoords();
    ChunkMeshGenerator::Request request;

    {
        std::unique_lock<std::mutex> lock{mChunksMutex};
        mChunks.emplace(coordinates, std::move(chunk));

        request.chunk = &mChunks.at(coordinates);
    }

    {
        std::unique_lock<std::mutex> lock{mEntitiesMutex};
        Engine::Entity entity = mEntities.at(coordinates);
        request.component = entity.Get<ShadedMesh>();
    }

    request.resultFunction = [this, coordinates]() {
        LOG_INFO("Generated chunk");

        /*
        std::vector<Voxel::Data> voxels;
        voxels.push_back(Voxel::Data{glm::vec3(0, 1, 0), glm::vec4(0, 255, 0, 1)});

        std::unique_lock<std::mutex> mEntitiesMutex;
        Engine::Entity entity = mEntities.at(coordinates);
        entity.Add<VoxelRender>(std::move(voxels));
        CHECK_GL_ERRORS();
        */
    };

    mChunkMeshGenerator->Add(request);
}

}; // namespace CubeWorld

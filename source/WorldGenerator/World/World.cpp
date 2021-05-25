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
World::World(Engine::EntityManager& entities, Engine::EventManager& events)
    : mEntityManager(entities)
    , mEventManager(events)
    , mEntity(entities.Create())
    , mChunkGenerator(new ChunkGenerator(mEventManager))
    , mChunkColliderGenerator(new ChunkColliderGenerator(mEventManager))
    , mChunkMeshGenerator(new ChunkMeshGenerator(mEventManager))
{
    mEntity.Add<Makeshift>([this](Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) {
        Update(entities, events, dt);
    });
}

///
///
///
World::~World()
{
    mQuitting = true;
    mChunkMeshGenerator.reset();
    mChunkGenerator.reset();
}

///
///
///
void World::Build()
{}

///
///
/// 
void World::Reset()
{
    /*
    mChunkGenerator.reset();
    mChunkMeshGenerator.reset();
    */

    mChunkGenerator->Clear();
    mChunkColliderGenerator->Clear();
    mChunkMeshGenerator->Clear();

    {
        std::unique_lock<std::mutex> lock{ mVersionMutex };
        mVersion++;
    }

    {
        std::unique_lock<std::mutex> lock{ mChunksMutex };
        mChunks.clear();
    }

    {
        std::unique_lock<std::mutex> lock{ mCompletedChunkCollisionMutex };
        mCompletedChunkCollision.clear();
    }

    /*
    {
        std::unique_lock<std::mutex> lock{ mEntitiesMutex };
        for (auto& [_, entity] : mEntities)
        {
            mEntityManager.Destroy(entity.GetID());
        }

        mEntities.clear();
    }

    mChunkGenerator.reset(new ChunkGenerator(mEventManager));
    mChunkMeshGenerator.reset(new ChunkMeshGenerator(mEventManager));
    */
}

///
///
///
Engine::Entity World::Create(int chunkX, int chunkY, int chunkZ)
{
    ChunkCoords coordinates{ chunkX, chunkY, chunkZ };

    Engine::Entity entity(&mEntityManager, Engine::Entity::ID(0, 0));
    if (mEntities.count(coordinates) == 0)
    {
        entity = mEntityManager.Create(
            float(chunkX) * kChunkSize - kChunkSize / 2,
            -32.0f,
            float(chunkZ) * kChunkSize - kChunkSize / 2
        );

        entity.Add<ShadedMesh>();
        entity.Get<ShadedMesh>()->renderType = GL_TRIANGLE_FAN;

        std::unique_lock<std::mutex> lock{ mEntitiesMutex };
        mEntities.emplace(coordinates, entity);
    }
    else
    {
        entity = mEntities.at(coordinates);
    }

    ChunkGenerator::Request request;
    request.coordinates = coordinates;
    request.resultFunction = std::bind(&World::OnChunkGenerated, this, mVersion, std::placeholders::_1);
    mChunkGenerator->Add(request);

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
        mChunks.emplace(coords, std::make_unique<Chunk>(coords));
    }

    return *mChunks.at(coords);
}

void World::Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt)
{
    CUBEWORLD_UNREFERENCED_PARAMETER(entities);
    CUBEWORLD_UNREFERENCED_PARAMETER(events);
    CUBEWORLD_UNREFERENCED_PARAMETER(dt);

    mChunkGenerator->Update();
    mChunkColliderGenerator->Update();
    mChunkMeshGenerator->Update();

    {
        std::unique_lock<std::mutex> lock{ mCompletedChunkCollisionMutex };
        for (auto& [coords, heights] : mCompletedChunkCollision)
        {
            assert(mEntities.count(coords) != 0);
            Engine::Entity e = mEntities.at(coords);

            auto body = e.Get<BulletPhysics::VoxelHeightfieldBody>();
            if (!body)
            {
                e.Add<BulletPhysics::VoxelHeightfieldBody>(
                    int16_t(kChunkSize + 1),
                    int16_t(kChunkSize + 1),
                    std::move(heights)
                );
            }
            else
            {
                //body->heights = std::move(heights);
            }
        }

        mCompletedChunkCollision.clear();
    }
}

/// 
/// 
/// 
void World::OnChunkGenerated(int version, std::unique_ptr<Chunk>&& chunk)
{
    if (mQuitting)
    {
        return;
    }

    {
        std::unique_lock<std::mutex> lock{ mVersionMutex };
        if (mVersion != version)
        {
            // Outdated chunk
            return;
        }
    }

    ChunkCoords coordinates = chunk->GetCoords();
    ChunkMeshGenerator::Request meshRequest;
    ChunkColliderGenerator::Request colliderRequest;

    {
        std::unique_lock<std::mutex> lock{mChunksMutex};
        if (mChunks.count(coordinates) != 0)
        {
            return;
        }
        mChunks.emplace(coordinates, std::move(chunk));

        meshRequest.chunk = mChunks.at(coordinates).get();
        colliderRequest.chunk = meshRequest.chunk;
    }

    {
        std::unique_lock<std::mutex> lock{mEntitiesMutex};
        Engine::Entity entity = mEntities.at(coordinates);
        meshRequest.component = entity.Get<ShadedMesh>();
    }

    mChunkMeshGenerator->Add(meshRequest);

    colliderRequest.resultFunction = std::bind(
        &World::OnChunkColliderGenerated, this,
        version,
        coordinates,
        std::placeholders::_1
    );
    mChunkColliderGenerator->Add(colliderRequest);
}

void World::OnChunkColliderGenerated(int version, ChunkCoords coordinates, std::vector<int16_t>&& heights)
{
    if (mQuitting)
    {
        return;
    }

    {
        std::unique_lock<std::mutex> lock{ mVersionMutex };
        if (mVersion != version)
        {
            // Outdated chunk
            return;
        }
    }

    std::unique_lock<std::mutex> lock{ mCompletedChunkCollisionMutex };
    mCompletedChunkCollision.emplace_back(coordinates, std::move(heights));
}

}; // namespace CubeWorld

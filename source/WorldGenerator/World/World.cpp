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
    , mChunkGenerator(new ChunkGenerator(mEventManager))
    , mChunkMeshGenerator(new ChunkMeshGenerator())
{}

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
    mChunkGenerator.reset();
    mChunkMeshGenerator.reset();

    {
        std::unique_lock<std::mutex> lock{ mVersionMutex };
        mVersion++;
    }

    {
        std::unique_lock<std::mutex> lock{ mChunksMutex };
        mChunks.clear();
    }

    {
        std::unique_lock<std::mutex> lock{ mEntitiesMutex };
        for (auto& [_, entity] : mEntities)
        {
            mEntityManager.Destroy(entity.GetID());
        }

        mEntities.clear();
    }

    mChunkGenerator.reset(new ChunkGenerator(mEventManager));
    mChunkMeshGenerator.reset(new ChunkMeshGenerator());
}

///
///
///
Engine::Entity World::Create(int chunkX, int chunkY, int chunkZ)
{
    ChunkCoords coordinates{chunkX, chunkY, chunkZ};

    Engine::Entity entity = mEntityManager.Create(
        float(chunkX) * kChunkSize,
        kChunkHeight / -2,
        float(chunkZ) * kChunkSize
    );

    entity.Add<ShadedMesh>();
    entity.Get<ShadedMesh>()->renderType = GL_TRIANGLE_FAN;

    std::unique_lock<std::mutex> lock{mEntitiesMutex};
    mEntities.emplace(coordinates, entity);

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
        mChunks.emplace(coords, coords);
    }

    return mChunks.at(coords);
}

void World::Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt)
{
    CUBEWORLD_UNREFERENCED_PARAMETER(entities);
    CUBEWORLD_UNREFERENCED_PARAMETER(events);
    CUBEWORLD_UNREFERENCED_PARAMETER(dt);

    mChunkGenerator->Update();
    mChunkMeshGenerator->Update();
}

/// 
/// 
/// 
void World::OnChunkGenerated(int version, Chunk&& chunk)
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

    request.resultFunction = [/*this, coordinates*/]() {
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

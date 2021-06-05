// By Thomas Steinke

#pragma once

#include <Engine/Core/Input.h>
#include <Engine/System/System.h>

#include <WorldGenerator/World/World.h>

namespace CubeWorld
{

struct ChunkSpawnSource : public Engine::Component<ChunkSpawnSource>
{
    ChunkSpawnSource() {}
    ChunkSpawnSource(float radius) : radius(radius) {}
    ChunkSpawnSource(const BindingProperty& data);
    
    float radius = 1024;
};

class ChunkManagementSystem : public Engine::System<ChunkManagementSystem>
{
public:
    ChunkManagementSystem(World* world) : mWorld(world) {}
    ~ChunkManagementSystem() {}

    void Configure(Engine::EntityManager&, Engine::EventManager&) override;
    void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

private:
    World* mWorld;
};

}; // namespace CubeWorld

namespace meta
{

using CubeWorld::ChunkSpawnSource;

template<>
inline auto registerMembers<ChunkSpawnSource>()
{
    return members(
        member("radius", &ChunkSpawnSource::radius)
    );
}

}; // namespace meta

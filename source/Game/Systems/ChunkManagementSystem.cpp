// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "ChunkManagementSystem.h"

namespace CubeWorld
{

ChunkSpawnSource::ChunkSpawnSource(const BindingProperty& data)
{
    Binding::deserialize(*this, data);
}

void ChunkManagementSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
}

void ChunkManagementSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
    entities.Each<Engine::Transform, ChunkSpawnSource>([&](Engine::Transform& transform, ChunkSpawnSource& source) {
        glm::vec3 pos = transform.GetAbsolutePosition();

        int32_t minX = int32_t(std::floor((pos.x - source.radius) / kChunkSize));
        int32_t maxX = int32_t(std::floor((pos.x + source.radius) / kChunkSize));
        int32_t minZ = int32_t(std::floor((pos.z - source.radius) / kChunkSize));
        int32_t maxZ = int32_t(std::floor((pos.z + source.radius) / kChunkSize));
        int32_t centerX = (minX + maxX) / 2;
        int32_t centerZ = (minZ + maxZ) / 2;

        // "Spiral out" from the middle, to ensure that the middle chunk is priority.
        for (int32_t k = 0; k <= (maxX - minX) / 2; ++k)
        {
            for (int32_t n = 0; n <= k; ++n)
            {
                mWorld->EnsureLoaded(centerX + n, 0, centerZ + k - n);
                mWorld->EnsureLoaded(centerX + n, 0, centerZ - k + n);
                mWorld->EnsureLoaded(centerX - n, 0, centerZ + k - n);
                mWorld->EnsureLoaded(centerX - n, 0, centerZ - k + n);
            }
        }
    });
}

}; // namespace CubeWorld

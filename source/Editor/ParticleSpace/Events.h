// By Thomas Steinke

#pragma once

#include <Engine/Event/Event.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Shared/Systems/SimpleParticleSystem.h>

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

// Clear the current ParticleEmitter for loading
DEFINE_NAMED_EVENT(ClearParticleEmitterEvent);

// Load a particular ParticleEmitter
DEFINE_NAMED_DATA_EVENT(LoadParticleEmitterEvent,
    std::string, filename
)

DEFINE_NAMED_EVENT(ParticleEmitterReadyEvent);

DEFINE_NAMED_DATA_EVENT(ParticleEmitterLoadedEvent,
    Engine::ComponentHandle<ParticleEmitter>, component
)
DEFINE_NAMED_DATA_EVENT(ParticleEmitterSavedEvent,
    Engine::ComponentHandle<ParticleEmitter>, component
)
DEFINE_NAMED_DATA_EVENT(ParticleEmitterModifiedEvent,
    Engine::ComponentHandle<ParticleEmitter>, component
)

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld

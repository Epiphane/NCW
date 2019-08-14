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
struct ClearParticleEmitterEvent : public Engine::Event<ClearParticleEmitterEvent> {};

// Load a particular ParticleEmitter
struct LoadParticleEmitterEvent : public Engine::Event<LoadParticleEmitterEvent>
{
   LoadParticleEmitterEvent(const std::string& filename) : filename(filename) {};

   std::string filename;
};

// Sent when a ParticleEmitter can be loaded
struct ParticleEmitterReadyEvent : public Engine::Event<ParticleEmitterReadyEvent> {};

// Sent when a ParticleEmitter is done loading
struct ParticleEmitterLoadedEvent : public Engine::Event<ParticleEmitterLoadedEvent>
{
   ParticleEmitterLoadedEvent(Engine::ComponentHandle<ParticleEmitter> component) : component(component) {};

   Engine::ComponentHandle<ParticleEmitter> component;
};

// Sent when a ParticleEmitter has been saved
struct ParticleEmitterSavedEvent : public Engine::Event<ParticleEmitterSavedEvent>
{
   ParticleEmitterSavedEvent(Engine::ComponentHandle<ParticleEmitter> component) : component(component) {};

   Engine::ComponentHandle<ParticleEmitter> component;
};

// Sent when a ParticleEmitter has been modified
struct ParticleEmitterModifiedEvent : public Engine::Event<ParticleEmitterModifiedEvent>
{
   ParticleEmitterModifiedEvent(Engine::ComponentHandle<ParticleEmitter> component) : component(component) {};

   Engine::ComponentHandle<ParticleEmitter> component;
};

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld

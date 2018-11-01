// By Thomas Steinke

#pragma once

#include <Engine/Event/Event.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Shared/Systems/AnimationSystem.h>

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

struct ModelLoadedEvent : public Engine::Event<ModelLoadedEvent>
{
   ModelLoadedEvent(Engine::ComponentHandle<AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<AnimatedSkeleton> component;
};

struct ModelSavedEvent : public Engine::Event<ModelSavedEvent>
{
   ModelSavedEvent(Engine::ComponentHandle<AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<AnimatedSkeleton> component;
};

struct ModelModifiedEvent : public Engine::Event<ModelModifiedEvent>
{
   ModelModifiedEvent(Engine::ComponentHandle<AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<AnimatedSkeleton> component;
};

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld

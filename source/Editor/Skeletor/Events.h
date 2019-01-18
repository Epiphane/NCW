// By Thomas Steinke

#pragma once

#include <Engine/Event/Event.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Shared/Systems/AnimationSystem.h>

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

struct SkeletonLoadedEvent : public Engine::Event<SkeletonLoadedEvent>
{
   SkeletonLoadedEvent(Engine::ComponentHandle<AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<AnimatedSkeleton> component;
};

struct SkeletonSavedEvent : public Engine::Event<SkeletonSavedEvent>
{
   SkeletonSavedEvent(Engine::ComponentHandle<AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<AnimatedSkeleton> component;
};

struct SkeletonModifiedEvent : public Engine::Event<SkeletonModifiedEvent>
{
   SkeletonModifiedEvent(Engine::ComponentHandle<AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<AnimatedSkeleton> component;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

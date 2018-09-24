// By Thomas Steinke

#pragma once

#include <Engine/Event/Event.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Shared/Systems/AnimationSystem.h>

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

struct SkeletonLoadedEvent : public Engine::Event<SkeletonLoadedEvent>
{
   SkeletonLoadedEvent(Engine::ComponentHandle<Game::AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<Game::AnimatedSkeleton> component;
};

struct SkeletonSavedEvent : public Engine::Event<SkeletonSavedEvent>
{
   SkeletonSavedEvent(Engine::ComponentHandle<Game::AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<Game::AnimatedSkeleton> component;
};

struct SkeletonModifiedEvent : public Engine::Event<SkeletonModifiedEvent>
{
   SkeletonModifiedEvent(Engine::ComponentHandle<Game::AnimatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<Game::AnimatedSkeleton> component;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

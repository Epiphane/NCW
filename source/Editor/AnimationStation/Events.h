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

struct SkeletonClearedEvent : public Engine::Event<SkeletonClearedEvent> {};

struct AddSkeletonPartEvent : public Engine::Event<AddSkeletonPartEvent>
{
   AddSkeletonPartEvent(const std::string& filename) : filename(filename) {};

   std::string filename;
};

struct SkeletonLoadedEvent : public Engine::Event<SkeletonLoadedEvent>
{
   SkeletonLoadedEvent(Engine::ComponentHandle<AnimationController> component) : component(component) {};

   Engine::ComponentHandle<AnimationController> component;
};

struct SkeletonSavedEvent : public Engine::Event<SkeletonSavedEvent>
{
   SkeletonSavedEvent(Engine::ComponentHandle<AnimationController> component) : component(component) {};

   Engine::ComponentHandle<AnimationController> component;
};

struct SkeletonModifiedEvent : public Engine::Event<SkeletonModifiedEvent>
{
   SkeletonModifiedEvent(Engine::ComponentHandle<AnimationController> component) : component(component) {};

   Engine::ComponentHandle<AnimationController> component;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

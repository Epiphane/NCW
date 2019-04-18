// By Thomas Steinke

#pragma once

#include <Engine/Event/Event.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Shared/Components/DeprecatedSkeleton.h>

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

struct SkeletonClearedEvent : public Engine::Event<SkeletonClearedEvent> {};

struct AddSkeletonPartEvent : public Engine::Event<AddSkeletonPartEvent>
{
   AddSkeletonPartEvent(const std::string& filename) : filename(filename) {};

   std::string filename;
};

struct SkeletonLoadedEvent : public Engine::Event<SkeletonLoadedEvent>
{
   SkeletonLoadedEvent(Engine::ComponentHandle<DeprecatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<DeprecatedSkeleton> component;
};

struct SkeletonSavedEvent : public Engine::Event<SkeletonSavedEvent>
{
   SkeletonSavedEvent(Engine::ComponentHandle<DeprecatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<DeprecatedSkeleton> component;
};

struct SkeletonModifiedEvent : public Engine::Event<SkeletonModifiedEvent>
{
   SkeletonModifiedEvent(Engine::ComponentHandle<DeprecatedSkeleton> component) : component(component) {};

   Engine::ComponentHandle<DeprecatedSkeleton> component;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

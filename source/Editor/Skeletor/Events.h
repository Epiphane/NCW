// By Thomas Steinke

#pragma once

#include <Engine/Event/Event.h>
#include <Engine/Entity/ComponentHandle.h>
#include "SkeletonSystem.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{
// Suspend editing for serialization
struct SuspendEditingEvent : public Engine::Event<SuspendEditingEvent> {};

// Resume editability (after serialization)
struct ResumeEditingEvent : public Engine::Event<ResumeEditingEvent> {};

// Clear the current skeleton for loading
struct SkeletonClearedEvent : public Engine::Event<SkeletonClearedEvent> {};

// Add a part to the current skeleton
struct AddSkeletonPartEvent : public Engine::Event<AddSkeletonPartEvent>
{
   AddSkeletonPartEvent(const std::string& filename) : filename(filename) {};

   std::string filename;
};

struct SkeletonLoadedEvent : public Engine::Event<SkeletonLoadedEvent>
{
   SkeletonLoadedEvent(Engine::ComponentHandle<Skeleton> component) : component(component) {};

   Engine::ComponentHandle<Skeleton> component;
};

struct SkeletonSavedEvent : public Engine::Event<SkeletonSavedEvent>
{
   SkeletonSavedEvent(Engine::ComponentHandle<Skeleton> component) : component(component) {};

   Engine::ComponentHandle<Skeleton> component;
};

struct SkeletonModifiedEvent : public Engine::Event<SkeletonModifiedEvent>
{
   SkeletonModifiedEvent(Engine::ComponentHandle<Skeleton> component) : component(component) {};

   Engine::ComponentHandle<Skeleton> component;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

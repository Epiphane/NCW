// By Thomas Steinke

#pragma once

#include <Engine/Event/Event.h>

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class SkeletonLoadedEvent : public Engine::Event<SkeletonLoadedEvent>
{};

class SkeletonSavedEvent : public Engine::Event<SkeletonSavedEvent>
{};

class SkeletonModifiedEvent : public Engine::Event<SkeletonModifiedEvent>
{};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

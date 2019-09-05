// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <RGBDesignPatterns/Command.h>
#include <RGBDesignPatterns/Either.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Event.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/UI/UIElement.h>

#include "SkeletonSystem.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

using Bone = Skeleton::Bone;

class Dock : public Engine::UIElement {
public:
   const double kTimelineWidth = 512.0;

public:
   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

public:
   // Dock state actions
   void SetStance(const std::string& stance);
   void SetBone(const std::string& bone);

public:
   // Event handlers
   void Receive(const SuspendEditingEvent& evt);
   void Receive(const ResumeEditingEvent& evt);
   void Receive(const SkeletonClearedEvent& evt);
   void Receive(const SkeletonLoadedEvent& evt);
   void Receive(const Engine::ComponentAddedEvent<SkeletonCollection>& evt);

private:
   // State
   std::string mBone;
   std::string mStance;

   std::unique_ptr<Command> mScrubbing;
   Engine::ComponentHandle<Skeleton> mSkeleton;
   Engine::ComponentHandle<SkeletonCollection> mSkeletons;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

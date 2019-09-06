// By Thomas Steinke

#pragma once

#include <string>
#include <unordered_map>

#include <Engine/UI/UIElement.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/RectFilled.h>

#include "SimpleAnimationSystem.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class Sidebar : public UI::RectFilled {
public:
   Sidebar(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

private:
   // Actions
   void SetModified(bool modified);

   void LoadNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

   void DiscardChanges();

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<SimpleAnimationController>& evt);
   void Receive(const SkeletonModifiedEvent& evt);

private:
   // State
   std::string mFilename;
   Engine::ComponentHandle<SimpleAnimationController> mSkeleton;
   bool mModified;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

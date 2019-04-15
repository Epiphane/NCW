// By Thomas Steinke

#pragma once

#include <string>
#include <unordered_map>

#include <Engine/UI/UIElement.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/RectFilled.h>

#include "AnimationSystem.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

class Sidebar : public UI::RectFilled {
public:
   Sidebar(Engine::UIRoot* root, Engine::UIElement* parent);

private:
   // Actions
   void SetModified(bool modified);

   void LoadNewFile();
   void SaveNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

   void DiscardChanges();
   void Quit();

private:
   // Elements
   UI::TextButton* mSave;
   UI::TextButton* mQuit;

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<SkeletonCollection>& evt);
   void Receive(const Engine::ComponentAddedEvent<AnimatedSkeleton>& evt);
   void Receive(const SkeletonModifiedEvent& evt);

private:
   // State
   std::string mFilename;
   Engine::ComponentHandle<AnimatedSkeleton> mSkeleton;
   bool mModified;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

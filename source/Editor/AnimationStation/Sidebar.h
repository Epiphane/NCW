// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/UI/UIElement.h>
#include <Shared/UI/TextButton.h>

#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class Sidebar : public Engine::UIElement {
public:
   Sidebar(Engine::UIRoot* root, UIElement* parent);

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
   void Receive(const Engine::ComponentAddedEvent<AnimatedSkeleton>& evt);
   void Receive(const SkeletonModifiedEvent& evt);

private:
   // State
   std::string mFilename;
   Engine::ComponentHandle<AnimatedSkeleton> mSkeleton;
   bool mModified;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

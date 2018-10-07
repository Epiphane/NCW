// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Event.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/Event/NamedEvent.h>
#include <Shared/Systems/AnimationSystem.h>

#include "../UI/TextButton.h"
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
   TextButton* mSave;
   TextButton* mQuit;

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<Game::AnimatedSkeleton>& evt);
   void Receive(const SkeletonModifiedEvent& evt);

private:
   // State
   std::string mFilename;
   Engine::ComponentHandle<Game::AnimatedSkeleton> mSkeleton;
   bool mModified;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

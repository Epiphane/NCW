// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/UI/UIRoot.h>
#include "SkeletonSystem.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

class Sidebar : public Engine::UIElement {
public:
   Sidebar(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

private:
   // Actions
   void SetModified(bool modified);

   void LoadNewFile();
   void SaveNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<SkeletonCollection>& evt);
   void Receive(const Engine::ComponentAddedEvent<Skeleton>& evt);
   void Receive(const SkeletonModifiedEvent& evt);

private:
   // State
   std::string mFilename;
   Engine::ComponentHandle<Skeleton> mSkeleton;
   bool mModified;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

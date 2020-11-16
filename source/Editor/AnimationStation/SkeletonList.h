// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

#include <RGBDesignPatterns/Command.h>
#include <Engine/UI/UIElementDep.h>
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

//
// SkeletonList sits on the right side of the window and shows the current available skeletons.
//
// By selecting a skeleton, any changes (e.g. adding states) affect that skeleton.
// It also allows for enabling and disabling certain skeletons without having to load a different tree.
//
class SkeletonList : public UI::RectFilled {
public:
   SkeletonList(Engine::UIRootDep* root, Engine::UIElementDep* parent);

private:
   // Actions
   void SelectSkeleton(size_t index);
   void ToggleSkeletonActive(size_t index);

private:
   // Elements
   std::vector<UI::TextButton*> mButtons;
   size_t mActive;

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<SimpleAnimationController>& evt);
   void Receive(const Engine::ComponentAddedEvent<Skeleton>& evt);
   void Receive(const SkeletonClearedEvent& evt);

private:
   // State
   Engine::ComponentHandle<SimpleAnimationController> mController;
   std::vector<Engine::ComponentHandle<Skeleton>> mSkeletons;

private:
   //
   //
   //
   class SelectSkeletonCommand : public Command {
   public:
      SelectSkeletonCommand(SkeletonList* list, size_t index) : list(list), index(index) {};

      void Do() override;
      void Undo() override { Do(); }

   protected:
      SkeletonList* list;
      size_t index;
   };
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

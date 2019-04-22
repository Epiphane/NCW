// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <rhea/variable.hpp>
#include <Engine/Core/Bounded.h>
#include <RGBDesignPatterns/Command.h>
#include <RGBDesignPatterns/Either.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Event.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/Image.h>
#include <Shared/UI/NumDisplay.h>
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/ScrollBar.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/TextField.h>

#include "../UI/Scrubber.h"
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
using UI::Image;
using UI::NumDisplay;
using UI::RectFilled;
using UI::ScrollBar;
using UI::Text;
using UI::TextField;

class Dock : public RectFilled {
public:
   const double kTimelineWidth = 512.0;

public:
   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

public:
   // Dock state actions
   void SetStance(const std::string& stance);
   void SetBone(const std::string& bone);

public:
   // Event handlers
   void Receive(const SkeletonLoadedEvent& evt);
   void Receive(const Engine::ComponentAddedEvent<SkeletonCollection>& evt);

private:
   // State
   std::string mBone;
   std::string mStance;

   std::unique_ptr<Command> mScrubbing;
   Engine::ComponentHandle<Skeleton> mSkeleton;
   Engine::ComponentHandle<SkeletonCollection> mSkeletons;
   std::vector<std::string> mStances;

private:
   template <typename N>
   struct LabelAndScrubber {
      NumDisplay<N>* text;
      Scrubber<N>* scrubber;
   };

   // Bone inspector
   Text* mBoneName;
   LabelAndScrubber<float> mBonePos[3];
   LabelAndScrubber<float> mBoneRot[3];
   LabelAndScrubber<float> mBoneScl[3];

private:
   //
   //
   //
   class DockCommand : public Command {
   public:
      DockCommand(Dock* dock) : dock(dock) {};

   protected:
      Dock* dock;
   };

   //
   //
   //
   class NextBoneCommand : public DockCommand
   {
   public:
      using DockCommand::DockCommand;
      void Do() override;
      void Undo() override;
   };

   //
   //
   //
   using PrevBoneCommand = ReverseCommand<NextBoneCommand>;

   //
   //
   //
   class SetStateNameCommand : public DockCommand
   {
   public:
      SetStateNameCommand(Dock* dock, std::string name) : DockCommand(dock), name(name) {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      std::string name;
   };
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

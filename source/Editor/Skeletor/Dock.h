// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <rhea/variable.hpp>
#include <Engine/Core/Bounded.h>
#include <Engine/Core/Command.h>
#include <Engine/Core/Either.h>
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
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

using Bone = AnimatedSkeleton::Bone;
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
   void SetBone(const size_t& boneId);

public:
   // Event handlers
   void Receive(const SkeletonLoadedEvent& evt);
   void Receive(const Engine::ComponentAddedEvent<AnimatedSkeleton>& evt);

private:
   // State
   size_t mBone;
   std::unique_ptr<Command> mScrubbing;
   Engine::ComponentHandle<AnimatedSkeleton> mSkeleton;

private:
   template <typename N>
   struct LabelAndScrubber {
      NumDisplay<N>* text;
      Scrubber<N>* scrubber;
   };

   // Use a SubWindow, to allow for adding and removing elements without waiting until between frames.
   UIElement* mKeyframes;
   std::vector<std::pair<Image*, rhea::variable>> mKeyframeIcons;

   // Bone inspector
   Text* mBoneName;
   Text* mBoneParent;
   LabelAndScrubber<float> mBonePos[3];
   LabelAndScrubber<float> mBoneRot[3];

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
   class ParentBoneCommand : public DockCommand
   {
   public:
      using DockCommand::DockCommand;
      void Do() override;
      void Undo() override;
   private:
      size_t last;
   };

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
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
#include <Shared/Systems/AnimationSystem.h>
#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/Image.h>
#include <Shared/UI/NumDisplay.h>
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/ScrollBar.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/TextField.h>

#include "../UI/Scrubber.h"
#include "AnimationSystem.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

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
   using State = AnimationController::State;
   using Keyframe = AnimationController::Keyframe;
   using Bone = DeprecatedSkeleton::Bone;

   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

   void AddKeyframeIcon();
   void UpdateKeyframeIcons();

public:
   // Dock state actions
   void SetState(const size_t& index);
   void SetBone(const AnimationController::BoneID& boneId);
   void SetTime(double time);

public:
   // Reactions to the scrubbers
   void SetStateLength(double newValue, double oldValue);

private:
   // Helper functions
   State& GetCurrentState();
   Keyframe& GetCurrentKeyframe();

public:
   // Event handlers
   void Receive(const SkeletonLoadedEvent& evt);
   void Receive(const SkeletonSelectedEvent& evt);
   void Receive(const Engine::ComponentAddedEvent<AnimationController>& evt);
   void Receive(const Engine::ComponentAddedEvent<AnimationSystemController>& evt);

private:
   // State
   AnimationController::BoneID mBone;
   std::unique_ptr<Command> mScrubbing;
   size_t mSkeleton;
   Engine::ComponentHandle<AnimationController> mController;
   Engine::ComponentHandle<AnimationSystemController> mSystemControls;

private:
   // Layout and elements
   rhea::variable c1, c2, c3, c4;

   template <typename N>
   struct LabelAndScrubber {
      NumDisplay<N>* text;
      Scrubber<N>* scrubber;
   };

   // General state info
   Image* mPlay;
   Image* mPause;
   Image* mTick;
   ScrollBar* mScrubber;
   size_t mSelectedKeyframe;
   Scrubber<double>* mKeyframeTime;
   TextField* mStateName;
   LabelAndScrubber<double> mStateLength;
   NumDisplay<double>* mTime;

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
   struct NextStateCommand : public DockCommand
   {
      using DockCommand::DockCommand;
      void Do() override;
      void Undo() override;
   };

   //
   //
   //
   using PrevStateCommand = ReverseCommand<NextStateCommand>;

   //
   //
   //
   class AddStateCommand : public DockCommand
   {
   public:
      AddStateCommand(Dock* dock) : DockCommand(dock), afterCurrent(true) {};
      AddStateCommand(Dock* dock, State base) : DockCommand(dock), afterCurrent(true), state(base) {};
      void Do() override;
      void Undo() override;

   private:
      bool afterCurrent;

      State state{"", "", 0, 0, 1.0f, {}, {}};
   };

   //
   //
   //
   using RemoveStateCommand = ReverseCommand<AddStateCommand>;

   //
   //
   //
   class SetStateLengthCommand : public DockCommand
   {
   public:
      SetStateLengthCommand(Dock* dock, double value) : DockCommand(dock), value(value) {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      double value;
   };

   //
   //
   //
   class AddKeyframeCommand : public DockCommand
   {
   public:
      AddKeyframeCommand(Dock* dock) : DockCommand(dock)
      {
         keyframe.time = dock->mController->time;
      };
      void Do() override;
      void Undo() override;

   protected:
      size_t keyframeIndex;
      AnimationController::Keyframe keyframe{};
   };

   //
   //
   //
   struct RemoveKeyframeCommand : public ReverseCommand<AddKeyframeCommand>
   {
      RemoveKeyframeCommand(Dock* dock, size_t index) : ReverseCommand<AddKeyframeCommand>(dock)
      {
         keyframeIndex = index;
      };
   };

   //
   //
   //
   class SetKeyframeTimeCommand : public DockCommand
   {
   public:
      SetKeyframeTimeCommand(Dock* dock, double value) : DockCommand(dock), value(value) {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      double value;
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
   class ResetBoneCommand : public DockCommand
   {
   public:
      ResetBoneCommand(Dock* dock, glm::vec3 position, glm::vec3 rotation)
         : DockCommand(dock)
         , position(position)
         , rotation(rotation)
      {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      glm::vec3 position, rotation;
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

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

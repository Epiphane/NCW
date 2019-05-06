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
   using Bone = Skeleton::Bone;
   using Keyframe = SkeletonAnimations::Keyframe;
   using State = SimpleAnimationController::State;
   using Stance = SimpleAnimationController::Stance;

   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

   void AddKeyframeIcon();
   void UpdateKeyframeIcons();

public:
   // Dock state actions
   void SetState(const std::string& state);
   void SetBone(const size_t& boneId);
   void SetTime(double time);

public:
   // Reactions to the scrubbers
   void SetStateLength(double newValue, double oldValue);

private:
   // Helper functions
   State& GetCurrentState();
   Stance& GetCurrentStance();
   Keyframe& GetCurrentKeyframe();

public:
   // Event handlers
   void Receive(const SuspendEditingEvent& evt);
   void Receive(const ResumeEditingEvent& evt);
   void Receive(const SkeletonLoadedEvent& evt);
   void Receive(const SkeletonSelectedEvent& evt);
   void Receive(const Engine::ComponentAddedEvent<SimpleAnimationController>& evt);
   void Receive(const Engine::ComponentAddedEvent<AnimationSystemController>& evt);

private:
   // State
   size_t mBone;
   size_t mSkeleton;
   std::vector<std::string> mStates;

   std::unique_ptr<Command> mScrubbing;
   Engine::ComponentHandle<SimpleAnimationController> mController;
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
      AddStateCommand(Dock* dock) : DockCommand(dock) {};
      AddStateCommand(Dock* dock, State base) : DockCommand(dock), state(base) {};
      void Do() override;
      void Undo() override;

   private:
      State state{"", "", "", "", 1.0f, {}};
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
      SkeletonAnimations::Keyframe keyframe{};
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
      ResetBoneCommand(Dock* dock, size_t bone, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
         : DockCommand(dock)
         , boneId(bone)
         , position(position)
         , rotation(rotation)
         , scale(scale)
      {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      size_t boneId;
      glm::vec3 position, rotation, scale;
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

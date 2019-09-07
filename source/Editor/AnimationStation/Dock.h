// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <RGBDesignPatterns/Command.h>
#include <RGBDesignPatterns/Either.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Event.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/UI/UIElement.h>

#include "../Imgui/Scrubber.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class Dock : public Engine::UIElement
{
public:
   const double kTimelineWidth = 512.0;

public:
   using Bone = Skeleton::Bone;
   using Keyframe = SkeletonAnimations::Keyframe;
   using State = SimpleAnimationController::State;
   using Stance = SimpleAnimationController::Stance;

   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

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

   enum class ScrubType
   {
      Position,
      Rotation,
      Scale,
   };
   void OnScrub(ScrubType type, glm::vec3 oldValue);

private:
   // State
   size_t mBone;
   std::string mSkeleton;
   size_t mSelectedKeyframe;
   std::vector<std::string> mStates;

   std::unique_ptr<Command> mScrubbing;
   Engine::ComponentHandle<SimpleAnimationController> mController;
   Engine::ComponentHandle<AnimationSystemController> mSystemControls;

   ScrubberVec3 mScrubbers[3];

private:
   //
   //
   //
   class DockCommand : public Command
   {
   public:
      DockCommand(Dock* dock) : dock(dock){};

   protected:
      Dock* dock;
   };

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
      State state{"", "", "", "base", 1.0f, {}, {}};
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
      AddKeyframeCommand(Dock* dock) : DockCommand(dock), keyframeIndex(0)
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

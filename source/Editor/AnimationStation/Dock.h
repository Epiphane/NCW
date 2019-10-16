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
#include "../Imgui/TextField.h"
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
   using Bone = Skeleton::Bone;
   using Keyframe = SkeletonAnimations::Keyframe;
   using State = SimpleAnimationController::State;
   using Stance = SimpleAnimationController::Stance;

   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

public:
   // Dock state actions
   void SetState(const size_t& state);
   void SetBone(const size_t& boneId);

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

   Engine::ComponentHandle<SimpleAnimationController> mController;
   Engine::ComponentHandle<AnimationSystemController> mSystemControls;

   TextField mStateName;
   Scrubber<double> mKeyframeTimeScrubber;
   Scrubber<glm::vec3, float> mScrubbers[3];

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
      AddStateCommand(Dock* dock, size_t index) : DockCommand(dock), index(index) {};
      AddStateCommand(Dock* dock, size_t index, State base) : DockCommand(dock), index(index), state(base) {};
      void Do() override;
      void Undo() override;

   private:
      size_t index;
      State state{"", "", "", "base", 1.0f, {}, {}, {}, {}};
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
      SetStateLengthCommand(Dock* dock, const size_t& index, double value)
         : DockCommand(dock)
         , index(index)
         , value(value)
      {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      size_t index;
      double value;
   };

   //
   //
   //
   class SetStanceCommand : public DockCommand
   {
   public:
      SetStanceCommand(Dock* dock, const size_t& index, const std::string& stance)
         : DockCommand(dock)
         , index(index)
         , stance(stance)
      {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      size_t index;
      std::string stance;
   };

   //
   //
   //
   class AddKeyframeCommand : public DockCommand
   {
   public:
      AddKeyframeCommand(Dock* dock, const size_t& state);
      void Do() override;
      void Undo() override;

   protected:
      size_t stateIndex;
      size_t keyframeIndex;
      SkeletonAnimations::Keyframe keyframe{};
   };

   //
   //
   //
   struct RemoveKeyframeCommand : public ReverseCommand<AddKeyframeCommand>
   {
      RemoveKeyframeCommand(Dock* dock, const size_t& state, size_t index)
         : ReverseCommand<AddKeyframeCommand>(dock, state)
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
      SetKeyframeTimeCommand(Dock* dock, const size_t& state, size_t index, double value)
         : DockCommand(dock)
         , stateIndex(state)
         , index(index)
         , value(value)
      {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      size_t stateIndex;
      size_t index;
      double value;
   };

   //
   //
   //
   class ResetBoneCommand : public DockCommand
   {
   public:
      ResetBoneCommand(Dock* dock, const size_t& state, size_t keyframe, size_t bone, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
         : DockCommand(dock)
         , state(state)
         , keyframeIndex(keyframe)
         , boneId(bone)
         , position(position)
         , rotation(rotation)
         , scale(scale)
      {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      size_t state;
      size_t keyframeIndex;
      size_t boneId;
      glm::vec3 position, rotation, scale;
   };

   //
   //
   //
   class SetStateNameCommand : public DockCommand
   {
   public:
      SetStateNameCommand(Dock* dock, const size_t& index, const std::string& name) : DockCommand(dock), index(index), name(name) {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      size_t index;
      std::string name;
   };
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/Command.h>
#include <Engine/Core/Either.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Event.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>

#include "../UI/Image.h"
#include "../UI/Label.h"
#include "../UI/Scrubber.h"
#include "../UI/StationaryScrubber.h"
#include "../UI/SubWindow.h"
#include "../Systems/AnimationSystem.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class Dock : public SubWindow, public Engine::EventManager, public Engine::Receiver<Dock> {
public:
   using State = Game::AnimatedSkeleton::State;
   using Keyframe = Game::AnimatedSkeleton::Keyframe;
   using Bone = Game::AnimatedSkeleton::Bone;

   Dock(
      Bounded& parent,
      const Options& options
   );

   void Update(TIMEDELTA dt) override;

   void UpdateKeyframeIcons();
   void UpdateBoneInfo();

public:
   // Dock state actions
   void SetState(const size_t& index);
   void SetBone(const size_t& boneId);
   void SetFloat(Label* label, float value);
   void SetTime(double time);

   enum ScrubType {
      STATE_LENGTH,
      KEYFRAME_TIME,
      POS_X, POS_Y, POS_Z,
      ROT_X, ROT_Y, ROT_Z
   };
   void StartScrubbing(ScrubType type);
   void FinishScrubbing(double);
   void Scrub(ScrubType type, double amount);

private:
   // Helper functions
   State& GetCurrentState();

public:
   // Event handlers
   void Receive(const SkeletonLoadedEvent& evt);
   void Receive(const Engine::ComponentAddedEvent<Game::AnimatedSkeleton>& evt);
   void Receive(const Engine::ComponentAddedEvent<AnimationSystemController>& evt);

private:
   // State
   size_t mBone;
   std::unique_ptr<Command> mScrubbing;
   Engine::ComponentHandle<Game::AnimatedSkeleton> mSkeleton;
   Engine::ComponentHandle<AnimationSystemController> mController;

private:
   // General state info
   Image* mPlay;
   Image* mPause;
   Image* mTick;
   Scrubber* mScrubber;
   Label* mStateName;
   Label* mStateLength;
   Label* mTime;

   // Use a SubWindow, to allow for adding and removing elements without waiting until between frames.
   SubWindow* mKeyframes;
   std::vector<Image*> mKeyframeIcons;

   // Bone inspector
   Label* mBoneName;
   Label* mBoneParent;
   Label* mBonePosX;
   Label* mBonePosY;
   Label* mBonePosZ;
   Label* mBoneRotX;
   Label* mBoneRotY;
   Label* mBoneRotZ;

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
      void Do() override;
      void Undo() override;

   private:
      bool afterCurrent;
      State state{"", 1.0f, {}, {}};
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
         keyframe.time = dock->mSkeleton->time;
      };
      void Do() override;
      void Undo() override;

   protected:
      size_t keyframeIndex;
      Game::AnimatedSkeleton::Keyframe keyframe{};
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
   class SetBoneCommand : public DockCommand
   {
   public:
      SetBoneCommand(Dock* dock, glm::vec3 position, glm::vec3 rotation)
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

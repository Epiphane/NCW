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

public:
   // Dock state actions
   void NextState();
   void PrevState();
   void SetState(const size_t& index);

   // Set the time, and return a the index of a keyframe if it snaps.
   Either<size_t, void> SetTime(double time);

   void NextBone();
   void PrevBone();
   void ParentBone();

   // Skeleton modification actions
   void AddState(State state, bool afterCurrent);
   State RemoveState();
   void SetStateName(std::string name);
   void ChangeStateLength(double increment);
   
   size_t AddKeyframe(Keyframe keyframe);
   Keyframe RemoveKeyframe(size_t index);

   enum ScrubType {
      KEYFRAME_TIME,
      POS_X, POS_Y, POS_Z,
      ROT_X, ROT_Y, ROT_Z
   };
   void StartScrubbing(ScrubType type);
   void FinishScrubbing(double);
   void Scrub(ScrubType type, double amount);
   void ResetPosition();
   void ResetRotation();

private:
   // Helper functions
   State& GetCurrentState();

public:
   // Event handlers
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
   // Commands
   class DockCommand : public Command {
   public:
      DockCommand(Dock* dock) : dock(dock) {};

   protected:
      Dock* dock;
   };

   class NextStateCommand : public DockCommand
   {
   public:
      using DockCommand::DockCommand;
      void Do() override { dock->NextState(); }
      void Undo() override { dock->PrevState(); }
   };

   class PrevStateCommand : public NextStateCommand
   {
   public:
      using NextStateCommand::NextStateCommand;
      void Do() override { NextStateCommand::Undo(); }
      void Undo() override { NextStateCommand::Do(); }
   };

   class AddStateCommand : public DockCommand
   {
   public:
      AddStateCommand(Dock* dock) : DockCommand(dock), afterCurrent(true) {};
      void Do() override { dock->AddState(state, afterCurrent); }
      void Undo() override
      {
         afterCurrent = dock->mSkeleton->current > 0;
         state = dock->RemoveState();
      }

   private:
      bool afterCurrent;
      Game::AnimatedSkeleton::State state{
         "", 1.0f, {}, {}
      };
   };

   class RemoveStateCommand : public AddStateCommand
   {
   public:
      RemoveStateCommand(Dock* dock) : AddStateCommand(dock) {};
      void Do() override { AddStateCommand::Undo(); }
      void Undo() override { AddStateCommand::Do(); }
   };

   class ChangeStateLengthCommand : public DockCommand
   {
   public:
      ChangeStateLengthCommand(Dock* dock, float increment) : DockCommand(dock), increment(increment) {};
      void Do() override { dock->ChangeStateLength(increment); }
      void Undo() override { dock->ChangeStateLength(-increment); }

   private:
      float increment;
   };

   class AddKeyframeCommand : public DockCommand
   {
   public:
      AddKeyframeCommand(Dock* dock) : DockCommand(dock)
      {
         keyframe.time = dock->mSkeleton->time;
      };
      void Do() override { keyframeIndex = dock->AddKeyframe(keyframe); }
      void Undo() override { keyframe = dock->RemoveKeyframe(keyframeIndex); }

   protected:
      size_t keyframeIndex;
      Game::AnimatedSkeleton::Keyframe keyframe{};
   };

   class RemoveKeyframeCommand : public AddKeyframeCommand
   {
   public:
      RemoveKeyframeCommand(Dock* dock, size_t index) : AddKeyframeCommand(dock)
      {
         keyframeIndex = index;
      };
      void Do() override { AddKeyframeCommand::Undo(); }
      void Undo() override { AddKeyframeCommand::Do(); }
   };

   class SetKeyframeTimeCommand : public DockCommand
   {
   public:
      SetKeyframeTimeCommand(Dock* dock, double value) : DockCommand(dock), value(value) {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      double value;
   };

   class NextBoneCommand : public DockCommand
   {
   public:
      using DockCommand::DockCommand;
      void Do() override { dock->NextBone(); }
      void Undo() override { dock->PrevBone(); }
   };

   class PrevBoneCommand : public NextBoneCommand
   {
   public:
      using NextBoneCommand::NextBoneCommand;
      void Do() override { NextBoneCommand::Undo(); }
      void Undo() override { NextBoneCommand::Do(); }
   };

   class ParentBoneCommand : public DockCommand
   {
   public:
      using DockCommand::DockCommand;
      void Do() override { last = dock->mBone; dock->ParentBone(); }
      void Undo() override { dock->mBone = last; }
   private:
      size_t last;
   };

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

   class SetStateNameCommand : public DockCommand
   {
   public:
      SetStateNameCommand(Dock* dock, std::string name) : DockCommand(dock), name(name) {};
      void Do() override
      {
         last = dock->mSkeleton->states[dock->mSkeleton->current].name;
         dock->SetStateName(name);
      }
      void Undo() override { dock->SetStateName(last); }

   private:
      std::string last, name;
   };
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

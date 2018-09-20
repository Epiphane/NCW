// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/Command.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>

#include "../UI/Image.h"
#include "../UI/Label.h"
#include "../UI/Scrubber.h"
#include "../UI/SubWindow.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class Dock : public SubWindow {
public:
   Dock(
      Bounded& parent,
      const Options& options,
      MainState* state
   );

   void Update(TIMEDELTA dt) override;

   void NextState();
   void PrevState();
   void SetState(size_t state);
   size_t GetState();

   void AddState(Game::AnimatedSkeleton::State state, bool afterCurrent);
   Game::AnimatedSkeleton::State RemoveState();
   void ChangeStateLength(double increment);

   void SnapKeyframe(bool forward);
   
   void AddKeyframe(Game::AnimatedSkeleton::Keyframe keyframe);
   Game::AnimatedSkeleton::Keyframe RemoveKeyframe();
   void ChangeKeyframeTime(double increment);

   double GetTime();
   void SetTime(double time);

   void PlayAnimation();
   void PauseAnimation();

private:
   MainState* mState;

   Image* mPlay;
   Image* mPause;
   Image* mTick;
   Scrubber* mScrubber;
   Label* mStateName;
   Label* mStateLength;
   Label* mKeyframeIndex;
   Label* mTime;
};

class DockCommand : public Command
{
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
      afterCurrent = dock->GetState() != 0;
      state = dock->RemoveState();
   }

private:
   bool afterCurrent;
   Game::AnimatedSkeleton::State state{
      "", 1.0f, {Game::AnimatedSkeleton::Keyframe{0.0f,{},{},{}}}, {}
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

class SnapKeyframeCommand : public DockCommand
{
public:
   SnapKeyframeCommand(Dock* dock, bool forward) : DockCommand(dock), forward(forward) {};
   void Do() override { prevTime = dock->GetTime(); dock->SnapKeyframe(forward); }
   void Undo() override { dock->SetTime(prevTime); }

private:
   bool forward;
   double prevTime;
};

class AddKeyframeCommand : public DockCommand
{
public:
   AddKeyframeCommand(Dock* dock) : DockCommand(dock)
   {
      keyframe.time = dock->GetTime();
   };
   void Do() override
   {
      if (keyframe.time >= 0)
      {
         dock->AddKeyframe(keyframe);
      }
   }
   void Undo() override { keyframe = dock->RemoveKeyframe(); }

private:
   Game::AnimatedSkeleton::Keyframe keyframe{
      0.0f,{},{},{}
   };
};

class RemoveKeyframeCommand : public AddKeyframeCommand
{
public:
   RemoveKeyframeCommand(Dock* dock) : AddKeyframeCommand(dock) {};
   void Do() override { AddKeyframeCommand::Undo(); }
   void Undo() override { AddKeyframeCommand::Do(); }
};

class ChangeKeyframeTimeCommand : public DockCommand
{
public:
   ChangeKeyframeTimeCommand(Dock* dock, float increment) : DockCommand(dock), increment(increment) {};
   void Do() override { dock->ChangeKeyframeTime(increment); }
   void Undo() override { dock->ChangeKeyframeTime(-increment); }

private:
   double increment;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

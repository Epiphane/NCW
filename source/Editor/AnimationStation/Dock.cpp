// By Thomas Steinke

#include <Shared/Helpers/Asset.h>

#include "../Command/CommandStack.h"

#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

using Game::AnimatedSkeleton;

Dock::Dock(
   Bounded& parent,
   const Options& options,
   MainState* state
)
   : SubWindow(parent, options)
   , mState(state)
{
   const float EIGHT_X = 8.0f / GetWidth();
   const float EIGHT_Y = 8.0f / GetHeight();

   // Background
   {
      Image::Options imageOptions;
      imageOptions.x = 0.0f;
      imageOptions.y = 0.0f;
      imageOptions.z = 0.5f;
      imageOptions.w = 1.0f;
      imageOptions.h = 1.0f;
      imageOptions.filename = Asset::Image("EditorDock.png");
      Add<Image>(imageOptions);
   }

   // Left-hand labels (name, length, keyframe info)
   {
      Label::Options labelOptions;
      labelOptions.x = 19 * EIGHT_X;
      labelOptions.y = 1.0f - 5 * EIGHT_Y;
      labelOptions.w = 20 * EIGHT_X;
      labelOptions.h = 2 * EIGHT_Y;
      labelOptions.text = "Walk away from the thing";
      Add<Label>(labelOptions);

      labelOptions.y -= 5 * EIGHT_Y;
      labelOptions.text = "0.0";
      Add<Label>(labelOptions);

      labelOptions.y -= 15 * EIGHT_Y;
      labelOptions.text = "1";
      Add<Label>(labelOptions);

      labelOptions.y -= 5 * EIGHT_Y;
      labelOptions.text = "0.0";
      Add<Label>(labelOptions);
   }

   // State buttons
   {
      Image::Options imageOptions;
      imageOptions.x = 40 * EIGHT_X;
      imageOptions.y = 1.0f - 5 * EIGHT_Y;
      imageOptions.w = 19.0f / GetWidth();
      imageOptions.h = 19.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "button_left";
      imageOptions.hoverImage = "hover_button_left";
      imageOptions.pressImage = "press_button_left";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<PrevStateCommand>(this); };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_right";
      imageOptions.hoverImage = "hover_button_right";
      imageOptions.pressImage = "press_button_right";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<NextStateCommand>(this); };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_add";
      imageOptions.hoverImage = "hover_button_add";
      imageOptions.pressImage = "press_button_add";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<AddStateCommand>(this); };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_remove";
      imageOptions.hoverImage = "hover_button_remove";
      imageOptions.pressImage = "press_button_remove";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<RemoveStateCommand>(this); };
      Add<Image>(imageOptions);
   }

   // State length buttons
   {
      Image::Options imageOptions;
      imageOptions.x = 30 * EIGHT_X;
      imageOptions.y = 1.0f - 10 * EIGHT_Y;
      imageOptions.w = 19.0f / GetWidth();
      imageOptions.h = 19.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "button_subtract";
      imageOptions.hoverImage = "hover_button_subtract";
      imageOptions.pressImage = "press_button_subtract";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<ChangeStateLengthCommand>(this, -0.1f); };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_add";
      imageOptions.hoverImage = "hover_button_add";
      imageOptions.pressImage = "press_button_add";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<ChangeStateLengthCommand>(this, 0.1f); };
      Add<Image>(imageOptions);
   }

   // Keyframe buttons
   {
      Image::Options imageOptions;
      imageOptions.x = 30 * EIGHT_X;
      imageOptions.y = 1.0f - 25 * EIGHT_Y;
      imageOptions.w = 42.0f / GetWidth();
      imageOptions.h = 19.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "button_prev";
      imageOptions.hoverImage = "hover_button_prev";
      imageOptions.pressImage = "press_button_prev";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<SnapKeyframeCommand>(this, false); };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_next";
      imageOptions.hoverImage = "hover_button_next";
      imageOptions.pressImage = "press_button_next";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<SnapKeyframeCommand>(this, true); };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.w = 19.0f / GetWidth();
      imageOptions.image = "button_add";
      imageOptions.hoverImage = "hover_button_add";
      imageOptions.pressImage = "press_button_add";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<AddKeyframeCommand>(this); };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_remove";
      imageOptions.hoverImage = "hover_button_remove";
      imageOptions.pressImage = "press_button_remove";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<RemoveKeyframeCommand>(this); };
      Add<Image>(imageOptions);
   }

   // Keyframe time buttons
   {
      Image::Options imageOptions;
      imageOptions.x = 30 * EIGHT_X;
      imageOptions.y = 1.0f - 30 * EIGHT_Y;
      imageOptions.w = 19.0f / GetWidth();
      imageOptions.h = 19.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "button_left";
      imageOptions.hoverImage = "hover_button_left";
      imageOptions.pressImage = "press_button_left";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<ChangeKeyframeTimeCommand>(this, -0.1f); };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_right";
      imageOptions.hoverImage = "hover_button_right";
      imageOptions.pressImage = "press_button_right";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<ChangeKeyframeTimeCommand>(this, 0.1f); };
      Add<Image>(imageOptions);
   }

   // Playback controls
   {
      Image::Options imageOptions;
      imageOptions.x = 60 * EIGHT_X;
      imageOptions.y = 1.0f - 6 * EIGHT_Y;
      imageOptions.w = 38.0f / GetWidth();
      imageOptions.h = 38.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "button_pause";
      imageOptions.hoverImage = "hover_button_pause";
      imageOptions.pressImage = "press_button_pause";
      imageOptions.onClick = std::bind(&Dock::PauseAnimation, this);
      mPause = Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_play";
      imageOptions.hoverImage = "hover_button_play";
      imageOptions.pressImage = "press_button_play";
      imageOptions.onClick = std::bind(&Dock::PlayAnimation, this);
      mPlay = Add<Image>(imageOptions);
      mPlay->SetActive(false);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_next_frame";
      imageOptions.hoverImage = "hover_button_next_frame";
      imageOptions.pressImage = "press_button_next_frame";
      imageOptions.onClick = std::bind(&MainState::TickAnimation, state, 0.16f);
      mTick = Add<Image>(imageOptions);
      mTick->SetActive(false);
   }

   // Keyframe scrubber
   {
      Scrubber::Options scrubberOptions;
      scrubberOptions.x = 3 * EIGHT_X;
      scrubberOptions.y = 1.0f - 20 * EIGHT_Y;
      scrubberOptions.w = 64 * EIGHT_X;
      scrubberOptions.h = 2 * EIGHT_Y;
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "frame_pointer";
      scrubberOptions.onPress = [&](double value) {
         PauseAnimation();
         Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
         SetTime(skeleton->states[skeleton->current].length * value);
      };
      scrubberOptions.onMove = [&](double value) {
         Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
         SetTime(skeleton->states[skeleton->current].length * value);
      };
      scrubberOptions.onRelease = scrubberOptions.onMove;
      mScrubber = Add<Scrubber>(scrubberOptions);
   }
}

void Dock::Update(TIMEDELTA dt)
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
   mScrubber->SetValue(skeleton->time / skeleton->states[skeleton->current].length);

   SubWindow::Update(dt);
}

void Dock::NextState()
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();

   if (++skeleton->current >= skeleton->states.size())
   {
      skeleton->current -= skeleton->states.size();
   }
   skeleton->next = skeleton->current;
}

void Dock::PrevState()
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
   if (skeleton->current-- == 0)
   {
      skeleton->current = skeleton->states.size() - 1;
   }
   skeleton->next = skeleton->current;
}

size_t Dock::GetState()
{
   return mState->GetPlayerSkeleton()->current;
}

void Dock::SetState(const size_t state)
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();

   skeleton->current = state;
}

void Dock::AddState(Game::AnimatedSkeleton::State state, bool afterCurrent)
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();

   skeleton->states.insert(skeleton->states.begin() + skeleton->current + (afterCurrent ? 1 : 0), state);
   if (afterCurrent)
   {
      skeleton->current++;
   }
}

Game::AnimatedSkeleton::State Dock::RemoveState()
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();

   AnimatedSkeleton::State& state = skeleton->states[skeleton->current];
   skeleton->states.erase(skeleton->states.begin() + skeleton->current);
   if (skeleton->current > 0)
   {
      skeleton->current--;
   }
   return state;
}

void Dock::ChangeStateLength(double increment)
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
   AnimatedSkeleton::State& state = skeleton->states[skeleton->current];

   double stretch = (state.length + increment) / state.length;
   state.length += increment;

   for (AnimatedSkeleton::Keyframe& keyframe : skeleton->states[skeleton->current].keyframes)
   {
      keyframe.time *= stretch;
   }
   skeleton->time *= stretch;
}

void Dock::SnapKeyframe(bool forward)
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
   AnimatedSkeleton::State& state = skeleton->states[skeleton->current];

   // Snap to the nearest keyframe if possible.
   SetTime(skeleton->time);

   size_t keyframeIndex = state.keyframes.size() - 1;
   while (skeleton->time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
   {
      keyframeIndex--;
   }

   if (forward)
   {
      if (keyframeIndex < state.keyframes.size() - 2)
      {
         skeleton->time = state.keyframes[keyframeIndex + 1].time;
      }
   }
   else
   {
      if (keyframeIndex > 0 && skeleton->time == state.keyframes[keyframeIndex].time)
      {
         skeleton->time = state.keyframes[keyframeIndex - 1].time;
      }
      else
      {
         skeleton->time = state.keyframes[keyframeIndex].time;
      }
   }
}

void Dock::AddKeyframe(Game::AnimatedSkeleton::Keyframe keyframe)
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
   AnimatedSkeleton::State& state = skeleton->states[skeleton->current];

   size_t keyframeIndex = state.keyframes.size() - 1;
   while (skeleton->time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
   {
      keyframeIndex--;
   }

   state.keyframes.insert(state.keyframes.begin() + keyframeIndex + 1, keyframe);
}

Game::AnimatedSkeleton::Keyframe Dock::RemoveKeyframe()
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
   AnimatedSkeleton::State& state = skeleton->states[skeleton->current];

   // Snap to the nearest keyframe if possible.
   SetTime(skeleton->time);

   size_t keyframeIndex = state.keyframes.size() - 1;
   while (skeleton->time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
   {
      keyframeIndex--;
   }

   if (skeleton->time != state.keyframes[keyframeIndex].time)
   {
      return {-1.0f,{},{},{}};
   }

   AnimatedSkeleton::Keyframe keyframe = state.keyframes[keyframeIndex];
   state.keyframes.erase(state.keyframes.begin() + keyframeIndex);
   return keyframe;
}

double Dock::GetTime()
{
   return mState->GetPlayerSkeleton()->time;
}

void Dock::SetTime(double time)
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
   AnimatedSkeleton::State& state = skeleton->states[skeleton->current];

   size_t keyframeIndex = state.keyframes.size() - 1;
   while (time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
   {
      keyframeIndex--;
   }

   if ((time - state.keyframes[keyframeIndex].time) / state.length < 0.02)
   {
      skeleton->time = state.keyframes[keyframeIndex].time;
   }
   else if ((state.keyframes[keyframeIndex + 1].time - time) / state.length < 0.02)
   {
      skeleton->time = state.keyframes[keyframeIndex + 1].time;
   }
   else
   {
      skeleton->time = time;
   }
}

void Dock::ChangeKeyframeTime(double increment)
{
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();
   AnimatedSkeleton::State& state = skeleton->states[skeleton->current];

   // Snap to the nearest keyframe if possible.
   SetTime(skeleton->time);

   size_t keyframeIndex = state.keyframes.size() - 1;
   while (skeleton->time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
   {
      keyframeIndex--;
   }

   if (skeleton->time != state.keyframes[keyframeIndex].time)
   {
      return;
   }

   state.keyframes[keyframeIndex].time += increment;
   skeleton->time += increment;
}

void Dock::PlayAnimation()
{
   mState->PlayAnimation();
   mPlay->SetActive(false);
   mTick->SetActive(false);
   mPause->SetActive(true);
}

void Dock::PauseAnimation()
{
   mState->PauseAnimation();
   mPlay->SetActive(true);
   mTick->SetActive(true);
   mPause->SetActive(false);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

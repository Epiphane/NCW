// By Thomas Steinke

#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
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
using State = Game::AnimatedSkeleton::State;
using Keyframe = Game::AnimatedSkeleton::Keyframe;
using Bone = Game::AnimatedSkeleton::Bone;

namespace
{

//
// Returns the index of the last keyframe. For example, if the animation is between
// keyframes 2 and 3, it will return 2.
//
size_t GetKeyframeIndex(State& state, double time)
{
   size_t keyframeIndex = state.keyframes.size() - 1;
   while (time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
   {
      keyframeIndex--;
   }

   return keyframeIndex;
}

///
///
///
Keyframe& GetKeyframe(State& state, double time)
{
   return state.keyframes[GetKeyframeIndex(state, time)];
}

}; // anonymous namespace

Dock::Dock(
   Bounded& parent,
   const Options& options
)
   : SubWindow(parent, options)
   , mBone(9)
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
      labelOptions.text = "N/A";
      labelOptions.onChange = [&](std::string value) { CommandStack::Instance()->Do<SetStateNameCommand>(this, value); };
      mStateName = Add<Label>(labelOptions);

      labelOptions.y -= 5 * EIGHT_Y;
      labelOptions.text = "0.0";
      labelOptions.onChange = nullptr;
      mStateLength = Add<Label>(labelOptions);

      labelOptions.y -= 15 * EIGHT_Y;
      labelOptions.text = "1";

      labelOptions.y -= 5 * EIGHT_Y;
      labelOptions.text = "0.0";
      mTime = Add<Label>(labelOptions);
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
      StationaryScrubber::Options scrubberOptions;
      scrubberOptions.x = 30 * EIGHT_X;
      scrubberOptions.y = 1.0f - 10 * EIGHT_Y;
      scrubberOptions.w = 64.0f / GetWidth();
      scrubberOptions.h = 14.0f / GetHeight();
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::STATE_LENGTH);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::STATE_LENGTH, std::placeholders::_1);
      scrubberOptions.onRelease = std::bind(&Dock::FinishScrubbing, this, std::placeholders::_1);
      scrubberOptions.alignCenter = false;
      Add<StationaryScrubber>(scrubberOptions);
   }

   // Keyframe buttons
   {
      Image::Options imageOptions;
      imageOptions.x = 30 * EIGHT_X;
      imageOptions.y = 1.0f - 25 * EIGHT_Y;
      imageOptions.w = 19.0f / GetWidth();
      imageOptions.h = 19.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");

      imageOptions.image = "button_add";
      imageOptions.hoverImage = "hover_button_add";
      imageOptions.pressImage = "press_button_add";
      imageOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mSkeleton->time);
         if (mSkeleton->time != keyframe.time)
         {
            CommandStack::Instance()->Do<AddKeyframeCommand>(this);
         }
      };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_remove";
      imageOptions.hoverImage = "hover_button_remove";
      imageOptions.pressImage = "press_button_remove";
      imageOptions.onClick = [&]() {
         State& state = GetCurrentState();
         size_t index = GetKeyframeIndex(state, mSkeleton->time);
         if (mSkeleton->time == state.keyframes[index].time)
         {
            CommandStack::Instance()->Do<RemoveKeyframeCommand>(this, index);
         }
      };
      Add<Image>(imageOptions);
   }

   // Keyframe time buttons
   {
      StationaryScrubber::Options scrubberOptions;
      scrubberOptions.x = 30 * EIGHT_X;
      scrubberOptions.y = 1.0f - 30 * EIGHT_Y;
      scrubberOptions.w = 64.0f / GetWidth();
      scrubberOptions.h = 14.0f / GetHeight();
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::KEYFRAME_TIME);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::KEYFRAME_TIME, std::placeholders::_1);
      scrubberOptions.onRelease = std::bind(&Dock::FinishScrubbing, this, std::placeholders::_1);
      scrubberOptions.alignCenter = false;
      Add<StationaryScrubber>(scrubberOptions);
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
      imageOptions.onClick = [&]() { mController->paused = true; };
      mPause = Add<Image>(imageOptions);

      imageOptions.image = "button_play";
      imageOptions.hoverImage = "hover_button_play";
      imageOptions.pressImage = "press_button_play";
      imageOptions.onClick = [&]() { mController->paused = false; };
      mPlay = Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_next_frame";
      imageOptions.hoverImage = "hover_button_next_frame";
      imageOptions.pressImage = "press_button_next_frame";
      imageOptions.onClick = [&]() { mController->nextTick = 0.1; };
      mTick = Add<Image>(imageOptions);
   }

   // Playback controls
   {
      Image::Options imageOptions;
      imageOptions.x = 60 * EIGHT_X;
      imageOptions.y = 1.0f - 11 * EIGHT_Y;
      imageOptions.w = 19.0f / GetWidth();
      imageOptions.h = 19.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "button_left";
      imageOptions.hoverImage = "hover_button_left";
      imageOptions.pressImage = "press_button_left";
      imageOptions.onClick = [&]() {
         mController->speed /= 2.0;
      };
      Add<Image>(imageOptions);

      imageOptions.x += imageOptions.w + EIGHT_X;
      imageOptions.image = "button_right";
      imageOptions.hoverImage = "hover_button_right";
      imageOptions.pressImage = "press_button_right";
      imageOptions.onClick = [&]() {
         mController->speed *= 2.0;
      };
      Add<Image>(imageOptions);
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
         mController->paused = true;
         SetTime(GetCurrentState().length * value);
      };
      scrubberOptions.onMove = scrubberOptions.onPress;
      scrubberOptions.onRelease = scrubberOptions.onPress;
      mScrubber = Add<Scrubber>(scrubberOptions);
   }

   // Bone information
   {
      Label::Options labelOptions;
      labelOptions.x = 82 * EIGHT_X;
      labelOptions.y = 1.0f - 5 * EIGHT_Y;
      labelOptions.w = 20 * EIGHT_X;
      labelOptions.h = 2 * EIGHT_Y;
      labelOptions.text = "Bone Name";
      mBoneName = Add<Label>(labelOptions);

      Image::Options imageOptions;
      imageOptions.x = labelOptions.x - 3 * EIGHT_X;
      imageOptions.y = labelOptions.y;
      imageOptions.w = 19.0f / GetWidth();
      imageOptions.h = 19.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "button_left";
      imageOptions.hoverImage = "hover_button_left";
      imageOptions.pressImage = "press_button_left";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<PrevBoneCommand>(this); };
      Add<Image>(imageOptions);

      imageOptions.x = labelOptions.x + labelOptions.w;
      imageOptions.image = "button_right";
      imageOptions.hoverImage = "hover_button_right";
      imageOptions.pressImage = "press_button_right";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<NextBoneCommand>(this); };
      Add<Image>(imageOptions);

      labelOptions.x -= 7 * EIGHT_X;
      labelOptions.y -= 5 * EIGHT_Y;
      labelOptions.text = "Parent";
      Add<Label>(labelOptions);

      labelOptions.x += labelOptions.w;
      labelOptions.text = "Parent Bone";
      mBoneParent = Add<Label>(labelOptions);

      imageOptions.x = labelOptions.x - 3 * EIGHT_X;
      imageOptions.y = labelOptions.y;
      imageOptions.image = "button_up";
      imageOptions.hoverImage = "hover_button_up";
      imageOptions.pressImage = "press_button_up";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<ParentBoneCommand>(this); };
      Add<Image>(imageOptions);
   }

   // Bone Numbers
   {
      Label::Options labelOptions;
      labelOptions.x = 81 * EIGHT_X;
      labelOptions.y = 1.0f - 15 * EIGHT_Y;
      labelOptions.w = 8 * EIGHT_X;
      labelOptions.h = 2 * EIGHT_Y;
      labelOptions.text = "0.0";
      mBonePosX = Add<Label>(labelOptions);

      StationaryScrubber::Options scrubberOptions;
      scrubberOptions.x = labelOptions.x - 3 * EIGHT_X;
      scrubberOptions.y = labelOptions.y - 3 * EIGHT_Y;
      scrubberOptions.w = 64.0f / GetWidth();
      scrubberOptions.h = 14.0f / GetHeight();
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.onRelease = std::bind(&Dock::FinishScrubbing, this, std::placeholders::_1);

      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::POS_X);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::POS_X, std::placeholders::_1);
      scrubberOptions.alignCenter = false;
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.y -= 7 * EIGHT_Y;
      mBonePosY = Add<Label>(labelOptions);

      scrubberOptions.y = labelOptions.y - 3 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::POS_Y);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::POS_Y, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.y -= 7 * EIGHT_Y;
      mBonePosZ = Add<Label>(labelOptions);

      scrubberOptions.y = labelOptions.y - 3 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::POS_Z);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::POS_Z, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.x += 18 * EIGHT_X;
      labelOptions.y = 1.0f - 15 * EIGHT_Y;
      mBoneRotX = Add<Label>(labelOptions);

      scrubberOptions.x = labelOptions.x - 3 * EIGHT_X;
      scrubberOptions.y = labelOptions.y - 3 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::ROT_X);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::ROT_X, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.y -= 7 * EIGHT_Y;
      mBoneRotY = Add<Label>(labelOptions);

      scrubberOptions.y = labelOptions.y - 3 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::ROT_Y);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::ROT_Y, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.y -= 7 * EIGHT_Y;
      mBoneRotZ = Add<Label>(labelOptions);

      scrubberOptions.y = labelOptions.y - 3 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::ROT_Z);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::ROT_Z, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);
   }

   // Reset buttons
   {
      Image::Options imageOptions;
      imageOptions.x = 73 * EIGHT_X;
      imageOptions.y = 1.0f - 28 * EIGHT_Y;
      imageOptions.w = 35.0f / GetWidth();
      imageOptions.h = 32.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "reset";
      imageOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mSkeleton->time);
         if (mSkeleton->time == keyframe.time)
         {
            CommandStack::Instance()->Do<SetBoneCommand>(this, mSkeleton->bones[mBone].position, keyframe.rotations[mBone]);
         }
      };
      Add<Image>(imageOptions);

      imageOptions.x += 18 * EIGHT_X;
      imageOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mSkeleton->time);
         if (mSkeleton->time == keyframe.time)
         {
            CommandStack::Instance()->Do<SetBoneCommand>(this, keyframe.positions[mBone], mSkeleton->bones[mBone].rotation);
         }
      };
      Add<Image>(imageOptions);
   }

   // Container for keyframe icons
   {
      SubWindow::Options keyframeContainerOptions;
      keyframeContainerOptions.x = 19.0f / GetWidth();
      keyframeContainerOptions.y = 1.0f - 136.0f / GetHeight();
      keyframeContainerOptions.w = 512.0f / GetWidth();
      keyframeContainerOptions.h = 32.0f / GetHeight();
      mKeyframes = Add<SubWindow>(keyframeContainerOptions);
   }

   Subscribe<SkeletonLoadedEvent>(*this);
   Subscribe<Engine::ComponentAddedEvent<Game::AnimatedSkeleton>>(*this);
   Subscribe<Engine::ComponentAddedEvent<AnimationSystemController>>(*this);
}

///
///
///
void Dock::Receive(const SkeletonLoadedEvent& evt)
{
   mSkeleton = evt.component;
   SetState(0);
   SetTime(0);
   SetBone(0);
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<Game::AnimatedSkeleton>& evt)
{
   mSkeleton = evt.component;
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<AnimationSystemController>& evt)
{
   mController = evt.component;
}

///
///
///
State& Dock::GetCurrentState()
{
   return mSkeleton->states[mSkeleton->current];
}

///
///
///
void Dock::Update(TIMEDELTA dt)
{
   State& state = GetCurrentState();

   // Update the UI according to the current animation time
   mScrubber->SetValue(mSkeleton->time / state.length);
   mTime->SetText(Format::FormatString("%.2f", mSkeleton->time));

   // Do it every frame, whatever
   UpdateBoneInfo();

   // Update the play/pause buttons
   mPlay->SetActive(mController->paused);
   mTick->SetActive(mController->paused);
   mPause->SetActive(!mController->paused);

   SubWindow::Update(dt);
}

///
///
///
void Dock::UpdateKeyframeIcons()
{
   State& state = GetCurrentState();

   while (mKeyframeIcons.size() < state.keyframes.size())
   {
      Image::Options keyframeOptions;
      keyframeOptions.w = 10.0f / mKeyframes->GetWidth();
      keyframeOptions.filename = Asset::Image("EditorIcons.png");
      keyframeOptions.image = "keyframe";

      size_t keyframeIndex = mKeyframeIcons.size();
      keyframeOptions.onClick = [&, keyframeIndex]() {
         SetTime(GetCurrentState().keyframes[keyframeIndex].time);
      };

      mKeyframeIcons.push_back(mKeyframes->Add<Image>(keyframeOptions));
   }
   while (mKeyframeIcons.size() > state.keyframes.size())
   {
      Image* toRemove = mKeyframeIcons.back();
      mKeyframeIcons.pop_back();
      mKeyframes->Remove(toRemove);
   }
   for (size_t i = 0; i < mKeyframeIcons.size(); i++)
   {
      mKeyframeIcons[i]->SetOffset(glm::vec3(state.keyframes[i].time / state.length, 0, 0));
   }
}

///
///
///
void Dock::UpdateBoneInfo()
{
   const Bone& bone = mSkeleton->bones[mBone];

   SetFloat(mBonePosX, bone.position.x);
   SetFloat(mBonePosY, bone.position.y);
   SetFloat(mBonePosZ, bone.position.z);
   SetFloat(mBoneRotX, bone.rotation.x);
   SetFloat(mBoneRotY, bone.rotation.y);
   SetFloat(mBoneRotZ, bone.rotation.z);
}

///
///
///
void Dock::SetState(const size_t& index)
{
   mSkeleton->current = index;

   State& state = GetCurrentState();
   mStateName->SetText(state.name);
   mStateLength->SetText(Format::FormatString("%.1f", state.length));

   UpdateKeyframeIcons();
}

///
///
///
void Dock::SetBone(const size_t& boneId)
{
   mBone = boneId;

   // Update bone info
   Bone& bone = mSkeleton->bones[mBone];
   Bone& parent = mSkeleton->bones[mBone != 0 ? bone.parent : 0];
   mBoneName->SetText(bone.name);
   mBoneParent->SetText(parent.name);

   UpdateBoneInfo();
}

///
///
///
void Dock::SetFloat(Label* label, float value)
{
   if (std::abs(value) < 0.1) { value = 0; }
   label->SetText(Format::FormatString("%.1f", value));
}

///
///
///
void Dock::AddStateCommand::Do()
{
   if (state.keyframes.size() == 0)
   {
      Keyframe keyframe;
      std::vector<Bone>& bones = dock->mSkeleton->bones;
      std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.positions), [](const Bone& b) { return b.position; });
      std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.rotations), [](const Bone& b) { return b.rotation; });

      keyframe.time = 0;
      state.keyframes.push_back(keyframe);
   }

   dock->mSkeleton->states.insert(dock->mSkeleton->states.begin() + dock->mSkeleton->current + (afterCurrent ? 1 : 0), state);
   if (afterCurrent)
   {
      dock->SetState(dock->mSkeleton->current + 1);
   }
   dock->Emit<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::AddStateCommand::Undo()
{
   afterCurrent = dock->mSkeleton->current > 0;
   // Get current state as a copy not a reference
   state = dock->GetCurrentState();

   dock->mSkeleton->states.erase(dock->mSkeleton->states.begin() + dock->mSkeleton->current);
   if (afterCurrent)
   {
      dock->SetState(dock->mSkeleton->current - 1);
   }
   dock->Emit<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::SetStateLengthCommand::Do()
{
   State& state = dock->GetCurrentState();

   double stretch = value / state.length;
   double last = state.length;
   state.length = value;
   value = last;

   for (Keyframe& keyframe : state.keyframes)
   {
      keyframe.time *= stretch;
   }

   dock->SetTime(dock->mSkeleton->time * stretch);
   dock->mStateLength->SetText(Format::FormatString("%.1f", state.length));
   dock->Emit<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::SetStateNameCommand::Do()
{
   State& state = dock->GetCurrentState();
   std::string last = state.name;
   state.name = name;
   name = last;
   dock->mStateName->SetText(state.name);
   dock->Emit<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::AddKeyframeCommand::Do()
{
   State& state = dock->GetCurrentState();

   if (keyframe.positions.size() != dock->mSkeleton->bones.size())
   {
      keyframe.positions.clear();
      keyframe.rotations.clear();

      // Insert the CURRENT values of position and rotation at this timestamp.
      std::vector<Bone>& bones = dock->mSkeleton->bones;
      std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.positions), [](const Bone& b) { return b.position; });
      std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.rotations), [](const Bone& b) { return b.rotation; });
   }

   keyframeIndex = GetKeyframeIndex(state, dock->mSkeleton->time) + 1;
   state.keyframes.insert(state.keyframes.begin() + keyframeIndex, keyframe);
   dock->UpdateKeyframeIcons();
   dock->Emit<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::AddKeyframeCommand::Undo()
{
   State& state = dock->GetCurrentState();

   // Copy by value not reference
   keyframe = state.keyframes[keyframeIndex];
   state.keyframes.erase(state.keyframes.begin() + keyframeIndex);
   dock->UpdateKeyframeIcons();
   dock->Emit<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::SetTime(double time)
{
   State& state = GetCurrentState();

   size_t keyframeIndex = GetKeyframeIndex(state, time);
   Keyframe& prev = state.keyframes[keyframeIndex];

   if ((time - prev.time) / state.length < 0.02)
   {
      mSkeleton->time = prev.time;
   }
   else if (
      (keyframeIndex < state.keyframes.size() - 1) && 
      (state.keyframes[keyframeIndex + 1].time - time) / state.length < 0.02
   )
   {
      mSkeleton->time = state.keyframes[keyframeIndex + 1].time;
   }
   else if ((state.length - time) / state.length < 0.02)
   {
      mSkeleton->time = state.length;
   }
   else
   {
      mSkeleton->time = time;
   }

   UpdateBoneInfo();
}

///
///
///
void Dock::StartScrubbing(ScrubType type)
{
   State& state = GetCurrentState();

   Keyframe& keyframe = GetKeyframe(state, mSkeleton->time);

   switch (type)
   {
   case STATE_LENGTH:
      mScrubbing = std::make_unique<SetStateLengthCommand>(this, state.length);
      break;
   case KEYFRAME_TIME:
      if (mSkeleton->time == keyframe.time)
      {
         mScrubbing = std::make_unique<SetKeyframeTimeCommand>(this, keyframe.time);
      }
      break;
   case POS_X:
   case POS_Y:
   case POS_Z:
   case ROT_X:
   case ROT_Y:
   case ROT_Z:
      if (mSkeleton->time == keyframe.time)
      {
         mScrubbing = std::make_unique<SetBoneCommand>(this, keyframe.positions[mBone], keyframe.rotations[mBone]);
      }
      break;
   }
}

///
///
///
void Dock::FinishScrubbing(double)
{
   // Funky time: at this point, the current bone position/rotation
   // represents the NEW state, and mScrubbing represents a command
   // to set the bone to the OLD state. So we perform the command
   // twice, once immediately to revert to the old state, and then
   // again when it gets placed on the stack to go back to the new
   // state.
   if (mScrubbing)
   {
      mScrubbing->Do();
      CommandStack::Instance()->Do(std::move(mScrubbing));
   }
}

///
///
///
void Dock::Scrub(ScrubType type, double amount)
{
   if (!mScrubbing)
   {
      return;
   }

   State& state = GetCurrentState();
   size_t index = GetKeyframeIndex(state, mSkeleton->time);
   Keyframe& keyframe = state.keyframes[index];

   switch (type)
   {
   case STATE_LENGTH:
   {
      double stretch = (state.length + amount) / state.length;
      state.length += amount;

      for (Keyframe& frame : state.keyframes)
      {
         frame.time *= stretch;
      }

      SetTime(mSkeleton->time * stretch);
      mStateLength->SetText(Format::FormatString("%.2f", state.length));
   }
      break;
   case KEYFRAME_TIME:
      keyframe.time += amount * state.length / 4.0f;

      if (index == 0)
      {
         keyframe.time = 0;
      }
      else if (keyframe.time <= state.keyframes[index - 1].time + state.length * 0.01f)
      {
         keyframe.time = state.keyframes[index - 1].time + state.length * 0.01f;
      }
      else if (index < state.keyframes.size() - 1 && keyframe.time >= state.keyframes[index + 1].time - state.length * 0.01f)
      {
         keyframe.time = state.keyframes[index + 1].time - state.length * 0.01f;
      }
      else if (keyframe.time >= state.length * 0.99f)
      {
         keyframe.time = state.length * 0.99f;
      }

      mSkeleton->time = keyframe.time;
      break;
   case POS_X:
      keyframe.positions[mBone].x += (float)amount;
      SetFloat(mBonePosX, keyframe.positions[mBone].x);
      break;
   case POS_Y:
      keyframe.positions[mBone].y += (float)amount;
      SetFloat(mBonePosY, keyframe.positions[mBone].y);
      break;
   case POS_Z:
      keyframe.positions[mBone].z += (float)amount;
      SetFloat(mBonePosZ, keyframe.positions[mBone].z);
      break;
   case ROT_X:
      keyframe.rotations[mBone].x += 10 * (float)amount;
      SetFloat(mBoneRotX, keyframe.rotations[mBone].x);
      break;
   case ROT_Y:
      keyframe.rotations[mBone].y += 10 * (float)amount;
      SetFloat(mBoneRotY, keyframe.rotations[mBone].y);
      break;
   case ROT_Z:
      keyframe.rotations[mBone].z += 10 * (float)amount;
      SetFloat(mBoneRotZ, keyframe.rotations[mBone].z);
      break;
   }
}

///
///
///
void Dock::NextStateCommand::Do()
{
   if (dock->mSkeleton->current >= dock->mSkeleton->states.size() - 1)
   {
      dock->SetState(0);
   }
   else
   {
      dock->SetState(dock->mSkeleton->current + 1);
   }
}

///
///
///
void Dock::NextStateCommand::Undo()
{
   if (dock->mSkeleton->current == 0)
   {
      dock->SetState(dock->mSkeleton->states.size() - 1);
   }
   else
   {
      dock->SetState(dock->mSkeleton->current - 1);
   }
}

///
///
///
void Dock::SetKeyframeTimeCommand::Do()
{
   State& state = dock->GetCurrentState();
   Keyframe& keyframe = GetKeyframe(state, dock->mSkeleton->time);
   if (dock->mSkeleton->time != keyframe.time)
   {
      return;
   }

   double last = keyframe.time;
   dock->mSkeleton->time = keyframe.time = value;
   dock->UpdateKeyframeIcons();
   value = last;

   dock->Emit<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::NextBoneCommand::Do()
{
   if (dock->mBone >= dock->mSkeleton->bones.size() - 1)
   {
      dock->SetBone(0);
   }
   else
   {
      dock->SetBone(dock->mBone + 1);
   }
}

///
///
///
void Dock::NextBoneCommand::Undo()
{
   if (dock->mBone == 0)
   {
      dock->SetBone(dock->mSkeleton->bones.size() - 1);
   }
   else
   {
      dock->SetBone(dock->mBone - 1);
   }
}

///
///
///
void Dock::ParentBoneCommand::Do()
{
   last = dock->mBone;
   if (dock->mBone > 0)
   {
      dock->SetBone(dock->mSkeleton->bones[dock->mBone].parent);
   }
}

///
///
///
void Dock::ParentBoneCommand::Undo()
{
   dock->SetBone(last);
}

///
///
///
void Dock::SetBoneCommand::Do()
{
   State& state = dock->GetCurrentState();
   Keyframe& keyframe = GetKeyframe(state, dock->mSkeleton->time);
   if (dock->mSkeleton->time != keyframe.time)
   {
      return;
   }
   
   size_t bone = dock->mBone;
   glm::vec3 pos = keyframe.positions[bone];
   glm::vec3 rot = keyframe.rotations[bone];
   keyframe.positions[bone] = position;
   keyframe.rotations[bone] = rotation;
   position = pos;
   rotation = rot;

   dock->SetFloat(dock->mBonePosX, keyframe.positions[bone].x);
   dock->SetFloat(dock->mBonePosY, keyframe.positions[bone].y);
   dock->SetFloat(dock->mBonePosZ, keyframe.positions[bone].z);
   dock->SetFloat(dock->mBoneRotX, keyframe.rotations[bone].x);
   dock->SetFloat(dock->mBoneRotY, keyframe.rotations[bone].y);
   dock->SetFloat(dock->mBoneRotZ, keyframe.rotations[bone].z);

   dock->Emit<SkeletonModifiedEvent>(dock->mSkeleton);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

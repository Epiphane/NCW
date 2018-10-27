// By Thomas Steinke

#include <algorithm>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/Button.h>
#include <Shared/UI/RectFilled.h>

#include "../Command/CommandStack.h"

#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

using State = AnimatedSkeleton::State;
using Keyframe = AnimatedSkeleton::Keyframe;
using Bone = AnimatedSkeleton::Bone;
using Engine::UIElement;
using Engine::UIFrame;
using UI::Button;
using UI::RectFilled;

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

//
//
//
Keyframe& GetKeyframe(State& state, double time)
{
   return state.keyframes[GetKeyframeIndex(state, time)];
}

}; // anonymous namespace

Dock::Dock(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent)
   , mBone(9)
{
   // Background
   {
      RectFilled* bg = Add<RectFilled>(glm::vec4(0.2, 0.2, 0.2, 1));
      RectFilled* fg = Add<RectFilled>(glm::vec4(0, 0, 0, 1));

      UIFrame& fBackground = bg->GetFrame();
      UIFrame& fForeground = fg->GetFrame();
      root->AddConstraints({
         fBackground.left == mFrame.left,
         fBackground.right == mFrame.right,
         fBackground.top == mFrame.top,
         fBackground.bottom == mFrame.bottom,
         mFrame > fForeground,

         fForeground.left == fBackground.left,
         fForeground.right == fBackground.right - 2,
         fForeground.top == fBackground.top - 2,
         fForeground.bottom == fBackground.bottom + 2,
         fForeground > fBackground,
      });
   }

   // Columns, for making everything all organized.
   root->AddConstraints({
      c1 == mFrame.left + 32,
      c2 >= c1 + 120,
      c3 >= c2 + 100,
      c4 >= c3 + 140,
   });

   // State name
   UIElement* stateName = Add<UIElement>();
   UIFrame& fStateName = stateName->GetFrame();
   {
      UIFrame& fRow = fStateName;
      UIFrame& fLabel = stateName->Add<Text>(Text::Options{"Name"})->GetFrame();

      mStateName = stateName->Add<TextField>(TextField::Options{[&](std::string value) {
         CommandStack::Instance()->Do<SetStateNameCommand>(this, value);
      }});
      UIFrame& fValue = mStateName->GetFrame();

      root->AddConstraints({
         fStateName.left == c1,
         fStateName.top == mFrame.top - 32,
         fStateName.height == 19,

         fLabel.left == fRow.left,
         fLabel.right == c2,
         fLabel.top == fRow.top,
         fLabel.height == fRow.height,

         fValue.left == c2,
         fValue.right == c3,
         fValue.top == fRow.top,
         fValue.height == fRow.height,
      });

      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");
      buttonOptions.image = "button_left";
      buttonOptions.hoverImage = "hover_button_left";
      buttonOptions.pressImage = "press_button_left";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<PrevStateCommand>(this); };
      UIFrame& fPrevState = stateName->Add<Button>(buttonOptions)->GetFrame();

      buttonOptions.image = "button_right";
      buttonOptions.hoverImage = "hover_button_right";
      buttonOptions.pressImage = "press_button_right";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<NextStateCommand>(this); };
      UIFrame& fNextState = stateName->Add<Button>(buttonOptions)->GetFrame();

      buttonOptions.image = "button_add";
      buttonOptions.hoverImage = "hover_button_add";
      buttonOptions.pressImage = "press_button_add";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<AddStateCommand>(this); };
      UIFrame& fAddState = stateName->Add<Button>(buttonOptions)->GetFrame();

      buttonOptions.image = "button_remove";
      buttonOptions.hoverImage = "hover_button_remove";
      buttonOptions.pressImage = "press_button_remove";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<RemoveStateCommand>(this); };
      UIFrame& fRemState = stateName->Add<Button>(buttonOptions)->GetFrame();

      root->AddConstraints({
         fPrevState.left == c3,
         fPrevState.top == fRow.top,
         fPrevState.bottom == fRow.bottom,
         fNextState.left == fPrevState.right + 8,
         fNextState.top == fRow.top,
         fNextState.bottom == fRow.bottom,
         fAddState.left == fNextState.right + 8,
         fAddState.top == fRow.top,
         fAddState.bottom == fRow.bottom,
         fRemState.left == fAddState.right + 8,
         fRemState.top == fRow.top,
         fRemState.bottom == fRow.bottom,
      });
   }

   // State length
   UIElement* stateLength = Add<UIElement>();
   UIFrame& fStateLength = stateLength->GetFrame();
   {
      UIFrame& fRow = fStateLength;
      UIFrame& fLabel = stateLength->Add<Text>(Text::Options{"Length"})->GetFrame();

      mStateLength.text = stateLength->Add<NumDisplay<double>>(NumDisplay<double>::Options(1));
      UIFrame& fValue = mStateLength.text->GetFrame();

      Scrubber<double>::Options scrubberOptions;
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.min = 0.1;
      scrubberOptions.onChange = std::bind(&Dock::SetStateLength, this, std::placeholders::_1, std::placeholders::_2);
      scrubberOptions.sensitivity = 0.05;
      mStateLength.scrubber = stateLength->Add<Scrubber<double>>(scrubberOptions);
      UIFrame& fScrubber = mStateLength.scrubber->GetFrame();

      root->AddConstraints({
         fStateLength.left == c1,
         fStateLength.top == fStateName.bottom - 16,
         fStateLength.height == fStateName.height,

         fLabel.left == fRow.left,
         fLabel.right == c2,
         fLabel.top == fRow.top,
         fLabel.height == fRow.height,

         fValue.left == c2,
         fValue.right == c3,
         fValue.top == fRow.top,
         fValue.height == fRow.height,

         fScrubber.left == c3,
         fScrubber.top == fRow.top,
         fScrubber.bottom == fRow.bottom,
      });
   }

   // ScrollBar for setting the current time in the animation
   RectFilled* timeline = Add<RectFilled>(glm::vec4(1, 1, 1, 0.1));
   UIFrame& fTimeline = timeline->GetFrame();
   root->AddConstraints({
      fTimeline.left == c1,
      fTimeline.top == fStateLength.bottom - 32,
      fTimeline.width == kTimelineWidth,
      fTimeline.height == 32,
   });

   ScrollBar::Options scrollbarOptions;
   scrollbarOptions.filename = Asset::Image("EditorIcons.png");
   scrollbarOptions.image = "frame_pointer";
   scrollbarOptions.onChange = std::bind(&Dock::SetTime, this, std::placeholders::_1);
   mScrubber = Add<ScrollBar>(scrollbarOptions);
   UIFrame& fScrubber = mScrubber->GetFrame();
   root->AddConstraints({
      fScrubber.left == fTimeline.left,
      fScrubber.top == fTimeline.bottom - 8,
      fScrubber.width == fTimeline.width,
      fScrubber.height == 10,
   });

   // Container for keyframe icons. Not very important but it can't hurt.
   {
      mKeyframes = Add<UIElement>();
      UIFrame& fKeyframes = mKeyframes->GetFrame();
      root->AddConstraints({
         fKeyframes.left == fTimeline.left,
         fKeyframes.right == fTimeline.right,
         fKeyframes.top == fTimeline.top,
         fKeyframes.bottom == fTimeline.bottom,
      });
   }

   // Keyframe buttons
   UIElement* keyframe = Add<UIElement>();
   UIFrame& fKeyframe = keyframe->GetFrame();
   {
      UIFrame& fRow = fKeyframe;
      UIFrame& fLabel = keyframe->Add<Text>(Text::Options{"Keyframe"})->GetFrame();

      root->AddConstraints({
         fKeyframe.left == c1,
         fKeyframe.top == fScrubber.bottom - 32,
         fKeyframe.height == fStateLength.height,

         fLabel.left == fRow.left,
         fLabel.right == c2,
         fLabel.top == fRow.top,
         fLabel.height == fRow.height,
      });

      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");

      buttonOptions.image = "button_add";
      buttonOptions.hoverImage = "hover_button_add";
      buttonOptions.pressImage = "press_button_add";
      buttonOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mSkeleton->time);
         if (mSkeleton->time != keyframe.time)
         {
            CommandStack::Instance()->Do<AddKeyframeCommand>(this);
         }
      };
      UIFrame& fAddFrame = keyframe->Add<Button>(buttonOptions)->GetFrame();

      buttonOptions.image = "button_remove";
      buttonOptions.hoverImage = "hover_button_remove";
      buttonOptions.pressImage = "press_button_remove";
      buttonOptions.onClick = [&]() {
         State& state = GetCurrentState();
         size_t index = GetKeyframeIndex(state, mSkeleton->time);
         if (mSkeleton->time == state.keyframes[index].time)
         {
            CommandStack::Instance()->Do<RemoveKeyframeCommand>(this, index);
         }
      };
      UIFrame& fRemFrame = keyframe->Add<Button>(buttonOptions)->GetFrame();

      root->AddConstraints({
         fAddFrame.left == c3,
         fAddFrame.top == fRow.top,
         fAddFrame.bottom == fRow.bottom,
         fRemFrame.left == fAddFrame.right + 8,
         fRemFrame.top == fRow.top,
         fRemFrame.bottom == fRow.bottom,
      });
   }

   // State length
   UIElement* time = Add<UIElement>();
   UIFrame& fTime = time->GetFrame();
   {
      UIFrame& fRow = fTime;
      UIFrame& fLabel = time->Add<Text>(Text::Options{"Time"})->GetFrame();

      mTime = time->Add<NumDisplay<double>>(NumDisplay<double>::Options(2));
      UIFrame& fValue = mTime->GetFrame();

      Scrubber<double>::Options scrubberOptions;
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.sensitivity = 0.05;
      scrubberOptions.onChange = [&](double newValue, double oldValue) {
         State& state = GetCurrentState();
         size_t index = mSelectedKeyframe;
         Keyframe& keyframe = state.keyframes[index];

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
         mpRoot->Suggest(mKeyframeIcons[index].second, keyframe.time / state.length);
      };
      mKeyframeTime = time->Add<Scrubber<double>>(scrubberOptions);
      UIFrame& fScrubber = mKeyframeTime->GetFrame();

      root->AddConstraints({
         fTime.left == c1,
         fTime.top == fKeyframe.bottom - 16,
         fTime.height == fKeyframe.height,

         fLabel.left == fRow.left,
         fLabel.right == c2,
         fLabel.top == fRow.top,
         fLabel.height == fRow.height,

         fValue.left == c2,
         fValue.right == c3,
         fValue.top == fRow.top,
         fValue.height == fRow.height,

         fScrubber.left == c3,
         fScrubber.top == fRow.top,
         fScrubber.bottom == fRow.bottom,
      });
   }

   // Playback controls
   UIElement* playback = Add<UIElement>();
   UIFrame& fPlayback = playback->GetFrame();
   {
      UIFrame& fRow = fPlayback;
      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");

      buttonOptions.image = "button_play";
      buttonOptions.hoverImage = "hover_button_play";
      buttonOptions.pressImage = "press_button_play";
      buttonOptions.onClick = [&]() { mController->paused = false; };
      mPlay = playback->Add<Button>(buttonOptions);
      UIFrame& fPlay = mPlay->GetFrame();

      buttonOptions.image = "button_pause";
      buttonOptions.hoverImage = "hover_button_pause";
      buttonOptions.pressImage = "press_button_pause";
      buttonOptions.onClick = [&]() { mController->paused = true; };
      mPause = playback->Add<Button>(buttonOptions);
      UIFrame& fPause = mPause->GetFrame();

      buttonOptions.image = "button_next_frame";
      buttonOptions.hoverImage = "hover_button_next_frame";
      buttonOptions.pressImage = "press_button_next_frame";
      buttonOptions.onClick = [&]() { mController->nextTick = 0.1; };
      mTick = playback->Add<Button>(buttonOptions);
      UIFrame& fNextFrame = mTick->GetFrame();

      root->AddConstraints({
         fPlayback.left == c4,
         fPlayback.top == fStateName.top,
         fPlayback.height == 38,

         fPlay.left == c4,
         fPlay.top == fRow.top,
         fPlay.bottom == fRow.bottom,

         fPause.left == fPlay.left,
         fPause.top == fPlay.top,
         fPause.bottom == fPlay.bottom,

         fNextFrame.left == fPlay.right + 8,
         fNextFrame.top == fRow.top,
         fNextFrame.bottom == fRow.bottom,
      });
   }

   /*
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

   // Bone information
   {
      Text::Options textOptions;
      textOptions.x = 82 * EIGHT_X;
      textOptions.y = 1.0f - 5 * EIGHT_Y;
      textOptions.w = 20 * EIGHT_X;
      textOptions.h = 2 * EIGHT_Y;
      textOptions.text = "Bone Name";
      mBoneName = Add<Text>(textOptions);

      Image::Options imageOptions;
      imageOptions.x = textOptions.x - 3 * EIGHT_X;
      imageOptions.y = textOptions.y;
      imageOptions.w = 19.0f / GetWidth();
      imageOptions.h = 19.0f / GetHeight();
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "button_left";
      imageOptions.hoverImage = "hover_button_left";
      imageOptions.pressImage = "press_button_left";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<PrevBoneCommand>(this); };
      Add<Image>(imageOptions);

      imageOptions.x = textOptions.x + textOptions.w;
      imageOptions.image = "button_right";
      imageOptions.hoverImage = "hover_button_right";
      imageOptions.pressImage = "press_button_right";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<NextBoneCommand>(this); };
      Add<Image>(imageOptions);

      textOptions.x -= 7 * EIGHT_X;
      textOptions.y -= 5 * EIGHT_Y;
      textOptions.text = "Parent";
      Add<Text>(textOptions);

      textOptions.x += textOptions.w;
      textOptions.text = "Parent Bone";
      mBoneParent = Add<Text>(textOptions);

      imageOptions.x = textOptions.x - 3 * EIGHT_X;
      imageOptions.y = textOptions.y;
      imageOptions.image = "button_up";
      imageOptions.hoverImage = "hover_button_up";
      imageOptions.pressImage = "press_button_up";
      imageOptions.onClick = [&]() { CommandStack::Instance()->Do<ParentBoneCommand>(this); };
      Add<Image>(imageOptions);
   }

   // Bone Numbers
   {
      NumDisplay<float>::Options textOptions;
      textOptions.x = 81 * EIGHT_X;
      textOptions.y = 1.0f - 15 * EIGHT_Y;
      textOptions.w = 8 * EIGHT_X;
      textOptions.h = 2 * EIGHT_Y;
      textOptions.text = "0.0";
      textOptions.precision = 1;

      Scrubber<float>::Options scrubberOptions;
      scrubberOptions.x = textOptions.x - 3 * EIGHT_X;
      scrubberOptions.y = textOptions.y - 3 * EIGHT_Y;
      scrubberOptions.w = 64.0f / GetWidth();
      scrubberOptions.h = 14.0f / GetHeight();
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.onChange = [&](double, double) { Emit<SkeletonModifiedEvent>(mSkeleton); };

      mBonePosX.text = Add<NumDisplay<float>>(textOptions);
      mBonePosX.scrubber = Add<Scrubber<float>>(scrubberOptions);

      textOptions.y -= 7 * EIGHT_Y;
      scrubberOptions.y = textOptions.y - 3 * EIGHT_Y;
      mBonePosY.text = Add<NumDisplay<float>>(textOptions);
      mBonePosY.scrubber = Add<Scrubber<float>>(scrubberOptions);

      textOptions.y -= 7 * EIGHT_Y;
      scrubberOptions.y = textOptions.y - 3 * EIGHT_Y;
      mBonePosZ.text = Add<NumDisplay<float>>(textOptions);
      mBonePosZ.scrubber = Add<Scrubber<float>>(scrubberOptions);

      // Rotation column
      textOptions.x += 18 * EIGHT_X;
      textOptions.y = 1.0f - 15 * EIGHT_Y;

      scrubberOptions.x = textOptions.x - 3 * EIGHT_X;
      scrubberOptions.y = textOptions.y - 3 * EIGHT_Y;
      mBoneRotX.text = Add<NumDisplay<float>>(textOptions);
      mBoneRotX.scrubber = Add<Scrubber<float>>(scrubberOptions);

      textOptions.y -= 7 * EIGHT_Y;
      scrubberOptions.y = textOptions.y - 3 * EIGHT_Y;
      mBoneRotY.text = Add<NumDisplay<float>>(textOptions);
      mBoneRotY.scrubber = Add<Scrubber<float>>(scrubberOptions);

      textOptions.y -= 7 * EIGHT_Y;
      scrubberOptions.y = textOptions.y - 3 * EIGHT_Y;
      mBoneRotZ.text = Add<NumDisplay<float>>(textOptions);
      mBoneRotZ.scrubber = Add<Scrubber<float>>(scrubberOptions);
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
            CommandStack::Instance()->Do<ResetBoneCommand>(this, mSkeleton->bones[mBone].position, keyframe.rotations[mBone]);
         }
      };
      Add<Image>(imageOptions);

      imageOptions.x += 18 * EIGHT_X;
      imageOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mSkeleton->time);
         if (mSkeleton->time == keyframe.time)
         {
            CommandStack::Instance()->Do<ResetBoneCommand>(this, keyframe.positions[mBone], mSkeleton->bones[mBone].rotation);
         }
      };
      Add<Image>(imageOptions);
   }
   */

   root->Subscribe<SkeletonLoadedEvent>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<AnimatedSkeleton>>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<AnimationSystemController>>(*this);
}

///
///
///
void Dock::Receive(const SkeletonLoadedEvent& evt)
{
   mSkeleton = evt.component;
   SetState(0);
   SetTime(0);
   //SetBone(0);
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<AnimatedSkeleton>& evt)
{
   mSkeleton = evt.component;
   mScrubber->Bind(&mSkeleton->time);
   mTime->Bind(&mSkeleton->time);
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
Keyframe& Dock::GetCurrentKeyframe()
{
   return GetKeyframe(GetCurrentState(), mSkeleton->time);
}

///
///
///
void Dock::Update(TIMEDELTA dt)
{
   // Update the play/pause buttons
   mPlay->SetActive(mController->paused);
   mTick->SetActive(mController->paused);
   mPause->SetActive(!mController->paused);

   UIElement::Update(dt);
}

///
///
///
void Dock::UpdateKeyframeIcons()
{
   State& state = GetCurrentState();

   size_t nKeyframes = state.keyframes.size();
   while (mKeyframeIcons.size() < nKeyframes)
   {
      Image::Options keyframeOptions;
      keyframeOptions.filename = Asset::Image("EditorIcons.png");
      keyframeOptions.image = "keyframe";

      Image* image = mKeyframes->Add<Image>(keyframeOptions);
      image->SetName(Format::FormatString("Frame %1", mKeyframeIcons.size()));

      UIFrame& fImage = image->GetFrame();
      UIFrame& fKeyframes = mKeyframes->GetFrame();
      auto entry = std::make_pair(image, rhea::variable());
      mpRoot->AddEditVar(entry.second);
      mpRoot->AddConstraints({
         fImage > fKeyframes,
         fImage.top == fKeyframes.top,
         fImage.bottom == fKeyframes.bottom,
         fImage.left == fKeyframes.left + kTimelineWidth * entry.second - fImage.width / 2,
      });

      mKeyframeIcons.push_back(entry);
   }
   for (size_t i = 0; i < mKeyframeIcons.size(); i++)
   {
      auto& entry = mKeyframeIcons[i];
      if (i < nKeyframes)
      {
         entry.first->SetActive(true);
         double val = state.keyframes[i].time / state.length;
         mpRoot->Suggest(entry.second, val);
      }
      else
      {
         entry.first->SetActive(false);
      }
   }
}

///
///
///
void Dock::SetState(const size_t& index)
{
   mSkeleton->current = index;

   State& state = GetCurrentState();
   mStateName->SetText(state.name);
   mStateLength.text->Bind(&state.length);
   mStateLength.scrubber->Bind(&state.length);
   mScrubber->SetBounds(0, state.length);

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

   mBonePosX.text->Bind(&bone.position.x);
   mBonePosX.text->Bind(&bone.position.y);
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
   dock->SendEvent<SkeletonModifiedEvent>(dock->mSkeleton);
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
   dock->SendEvent<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::SetStateLength(double newValue, double oldValue)
{
   State& state = GetCurrentState();

   double stretch = newValue / oldValue;

   for (Keyframe& keyframe : state.keyframes)
   {
      keyframe.time *= stretch;
   }

   mScrubber->SetBounds(0, newValue);
   SetTime(mSkeleton->time * stretch);
   SendEvent<SkeletonModifiedEvent>(mSkeleton);
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
   dock->SendEvent<SkeletonModifiedEvent>(dock->mSkeleton);
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
   dock->SendEvent<SkeletonModifiedEvent>(dock->mSkeleton);
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
   dock->SendEvent<SkeletonModifiedEvent>(dock->mSkeleton);
}

///
///
///
void Dock::SetTime(double time)
{
   State& state = GetCurrentState();

   size_t keyframeIndex = GetKeyframeIndex(state, time);
   Keyframe& prev = state.keyframes[keyframeIndex];
   mSelectedKeyframe = 0;

   if ((time - prev.time) / state.length < 0.02)
   {
      mSkeleton->time = prev.time;
      mSelectedKeyframe = keyframeIndex;
   }
   else if (
      (keyframeIndex < state.keyframes.size() - 1) && 
      (state.keyframes[keyframeIndex + 1].time - time) / state.length < 0.02
   )
   {
      mSkeleton->time = state.keyframes[keyframeIndex + 1].time;
      mSelectedKeyframe = keyframeIndex + 1;
   }
   else if ((state.length - time) / state.length < 0.02)
   {
      mSkeleton->time = state.length;
   }
   else
   {
      mSkeleton->time = time;
   }

   mKeyframeTime->Bind(&state.keyframes[mSelectedKeyframe].time);
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

   dock->SendEvent<SkeletonModifiedEvent>(dock->mSkeleton);
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
void Dock::ResetBoneCommand::Do()
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

   dock->SendEvent<SkeletonModifiedEvent>(dock->mSkeleton);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

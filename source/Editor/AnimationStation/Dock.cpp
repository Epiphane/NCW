// By Thomas Steinke

#include <algorithm>
#include <Engine/Entity/EntityManager.h>
#include <Engine/UI/UIStackView.h>
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

using State = AnimationController::State;
using Keyframe = AnimationController::Keyframe;
using Bone = AnimatedSkeleton::Bone;
using Engine::UIElement;
using Engine::UIFrame;
using Engine::UIStackView;
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
   : RectFilled(root, parent, "AnimationStationDock", glm::vec4(0.2, 0.2, 0.2, 1))
   , mBone(0)
{
   RectFilled* foreground = Add<RectFilled>("AnimationStationDockFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainCenterTo(this);
   foreground->ConstrainDimensionsTo(this, -4);

   // Columns, for making everything all organized.
   root->AddConstraints({
      c1 == mFrame.left + 32,
      c2 >= c1 + 120,
      c3 >= c2 + 100,
      c4 >= c3 + 140,
   });
   
   UIStackView* dockStateInfo = foreground->Add<UIStackView>("DockStateInfo");
   dockStateInfo->ConstrainLeftAlignedTo(foreground, 30);
   dockStateInfo->ConstrainTopAlignedTo(foreground, 30);
   dockStateInfo->ConstrainWidth(kTimelineWidth);
   dockStateInfo->SetOffset(16.0);

   // These elements are used for lining up columns nicely
   Text* stateNameLabel = nullptr;
   Button* prevStateButton = nullptr;

   // State name
   UIStackView* stateName = dockStateInfo->Add<UIStackView>();
   stateName->SetVertical(false);
   stateName->SetOffset(8.0);
   stateName->ConstrainHeight(19);
   stateName->ConstrainLeftAlignedTo(dockStateInfo);
   {
      stateNameLabel = stateName->Add<Text>(Text::Options{"Name"});
      stateNameLabel->ConstrainTopAlignedTo(stateName);
      stateNameLabel->ConstrainHeightTo(stateName);

      mStateName = stateName->Add<TextField>(TextField::Options{[&](std::string value) {
         CommandStack::Instance()->Do<SetStateNameCommand>(this, value);
      }});
      mStateName->ConstrainTopAlignedTo(stateName);
      mStateName->ConstrainHeightTo(stateName);

      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");
      buttonOptions.image = "button_left";
      buttonOptions.hoverImage = "hover_button_left";
      buttonOptions.pressImage = "press_button_left";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<PrevStateCommand>(this); };
      prevStateButton = stateName->Add<Button>(buttonOptions);
      prevStateButton->ConstrainTopAlignedTo(stateName);
      prevStateButton->ConstrainHeightTo(stateName);

      buttonOptions.image = "button_right";
      buttonOptions.hoverImage = "hover_button_right";
      buttonOptions.pressImage = "press_button_right";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<NextStateCommand>(this); };
      Button* nextStateButton = stateName->Add<Button>(buttonOptions);
      nextStateButton->ConstrainTopAlignedTo(stateName);
      nextStateButton->ConstrainHeightTo(stateName);

      buttonOptions.image = "button_add";
      buttonOptions.hoverImage = "hover_button_add";
      buttonOptions.pressImage = "press_button_add";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<AddStateCommand>(this); };
      Button* addStateButton = stateName->Add<Button>(buttonOptions);
      addStateButton->ConstrainTopAlignedTo(stateName);
      addStateButton->ConstrainHeightTo(stateName);

      buttonOptions.image = "button_remove";
      buttonOptions.hoverImage = "hover_button_remove";
      buttonOptions.pressImage = "press_button_remove";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<RemoveStateCommand>(this); };
      Button* removeStateButton = stateName->Add<Button>(buttonOptions);
      removeStateButton->ConstrainTopAlignedTo(stateName);
      removeStateButton->ConstrainHeightTo(stateName);
   }

   // State Length
   UIStackView* stateLength = dockStateInfo->Add<UIStackView>();
   stateLength->SetVertical(false);
   stateLength->SetOffset(8.0);
   stateLength->ConstrainHeightTo(stateName);
   stateLength->ConstrainLeftAlignedTo(stateName);
   {
      Text* stateLengthLabel = stateLength->Add<Text>(Text::Options{"Length"});
      stateLengthLabel->ConstrainTopAlignedTo(stateLength);
      stateLengthLabel->ConstrainHeightTo(stateLength);

      mStateLength.text = stateLength->Add<NumDisplay<double>>(NumDisplay<double>::Options(1));
      mStateLength.text->ConstrainTopAlignedTo(stateLength);
      mStateLength.text->ConstrainHeightTo(stateLength);

      Scrubber<double>::Options scrubberOptions;
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.min = 0.1;
      scrubberOptions.onChange = std::bind(&Dock::SetStateLength, this, std::placeholders::_1, std::placeholders::_2);
      scrubberOptions.sensitivity = 0.05;
      mStateLength.scrubber = stateLength->Add<Scrubber<double>>(scrubberOptions);
      mStateLength.scrubber->ConstrainTopAlignedTo(stateLength);
      mStateLength.scrubber->ConstrainHeightTo(stateLength);

      // Column alignment
      stateLengthLabel->ConstrainLeftAlignedTo(stateNameLabel);
      mStateLength.text->ConstrainLeftAlignedTo(mStateName);
      mStateLength.scrubber->ConstrainLeftAlignedTo(prevStateButton);
   }

   // ScrollBar for setting the current time in the animation
   {
      RectFilled* timeline = dockStateInfo->Add<RectFilled>("AnimationTimeline", glm::vec4(1, 1, 1, 0.1));
      timeline->ConstrainLeftAlignedTo(stateName);
      timeline->ConstrainHeight(32);
      //timeline->ConstrainBelow(stateLength, 32);
      timeline->ConstrainWidth(kTimelineWidth);

      // Container for keyframe icons. Not very important but it can't hurt.
      mKeyframes = Add<UIElement>();
      mKeyframes->ConstrainLeftAlignedTo(timeline);
      mKeyframes->ConstrainTopAlignedTo(timeline);
      mKeyframes->ConstrainDimensionsTo(timeline);

      ScrollBar::Options scrollbarOptions;
      scrollbarOptions.filename = Asset::Image("EditorIcons.png");
      scrollbarOptions.image = "frame_pointer";
      scrollbarOptions.onChange = std::bind(&Dock::SetTime, this, std::placeholders::_1);
      mScrubber = dockStateInfo->Add<ScrollBar>(scrollbarOptions);
      mScrubber->ConstrainLeftAlignedTo(timeline);
      mScrubber->ConstrainWidthTo(timeline);
      mScrubber->ConstrainHeight(10);
   }

   // Keyframe buttons
   UIStackView* keyframe = dockStateInfo->Add<UIStackView>();
   keyframe->SetVertical(false);
   keyframe->SetOffset(8.0);
   keyframe->ConstrainHeightTo(stateName);
   keyframe->ConstrainLeftAlignedTo(stateName);
   {
      Text* label = keyframe->Add<Text>(Text::Options{"Keyframe"});
      label->ConstrainTopAlignedTo(keyframe);
      label->ConstrainHeightTo(keyframe);

      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");

      buttonOptions.image = "button_add";
      buttonOptions.hoverImage = "hover_button_add";
      buttonOptions.pressImage = "press_button_add";
      buttonOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mController->time);
         if (mController->time != keyframe.time)
         {
            CommandStack::Instance()->Do<AddKeyframeCommand>(this);
         }
      };
      Button* addFrameButton = keyframe->Add<Button>(buttonOptions);
      addFrameButton->ConstrainTopAlignedTo(keyframe);
      addFrameButton->ConstrainHeightTo(keyframe);

      buttonOptions.image = "button_remove";
      buttonOptions.hoverImage = "hover_button_remove";
      buttonOptions.pressImage = "press_button_remove";
      buttonOptions.onClick = [&]() {
         State& state = GetCurrentState();
         size_t index = GetKeyframeIndex(state, mController->time);
         if (mController->time == state.keyframes[index].time)
         {
            CommandStack::Instance()->Do<RemoveKeyframeCommand>(this, index);
         }
      };
      Button* removeFrameButton = keyframe->Add<Button>(buttonOptions);
      removeFrameButton->ConstrainTopAlignedTo(keyframe);
      removeFrameButton->ConstrainHeightTo(keyframe);

      // Column alignment
      label->ConstrainLeftAlignedTo(stateNameLabel);
      addFrameButton->ConstrainLeftAlignedTo(prevStateButton);
   }

   // State length
   UIStackView* time = dockStateInfo->Add<UIStackView>();
   time->SetVertical(false);
   time->SetOffset(8.0);
   time->ConstrainHeightTo(stateName);
   time->ConstrainLeftAlignedTo(stateName);
   {
      Text* label = time->Add<Text>(Text::Options{"Time"});
      label->ConstrainTopAlignedTo(time);
      label->ConstrainHeightTo(time);

      mTime = time->Add<NumDisplay<double>>(NumDisplay<double>::Options(2));
      mTime->ConstrainTopAlignedTo(time);
      mTime->ConstrainHeightTo(time);

      Scrubber<double>::Options scrubberOptions;
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.sensitivity = 0.05;
      scrubberOptions.onChange = [&](double, double) {
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

         mController->time = keyframe.time;
         mpRoot->Suggest(mKeyframeIcons[index].second, keyframe.time / state.length);
      };
      mKeyframeTime = time->Add<Scrubber<double>>(scrubberOptions);
      mKeyframeTime->ConstrainTopAlignedTo(time);
      mKeyframeTime->ConstrainHeightTo(time);

      // Column alignment
      label->ConstrainLeftAlignedTo(stateNameLabel);
      mTime->ConstrainLeftAlignedTo(mStateName);
      mKeyframeTime->ConstrainLeftAlignedTo(prevStateButton);
   }

   // Playback controls
   {
      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");

      buttonOptions.image = "button_play";
      buttonOptions.hoverImage = "hover_button_play";
      buttonOptions.pressImage = "press_button_play";
      buttonOptions.onClick = [&]() { mSystemControls->paused = false; };
      mPlay = Add<Button>(buttonOptions);
      mPlay->ConstrainHeight(38);
      mPlay->ConstrainTopAlignedTo(dockStateInfo);
      mPlay->ConstrainToRightOf(dockStateInfo);

      buttonOptions.image = "button_pause";
      buttonOptions.hoverImage = "hover_button_pause";
      buttonOptions.pressImage = "press_button_pause";
      buttonOptions.onClick = [&]() { mSystemControls->paused = true; };
      mPause = Add<Button>(buttonOptions);
      mPause->ConstrainDimensionsTo(mPlay);
      mPause->ConstrainTopAlignedTo(mPlay);
      mPause->ConstrainLeftAlignedTo(mPlay);

      UIStackView* playback = Add<UIStackView>();
      playback->SetVertical(false);
      playback->SetOffset(8.0);
      playback->ConstrainHeight(19);
      playback->ConstrainBelow(mPlay, 8);
      playback->ConstrainLeftAlignedTo(mPlay);

      buttonOptions.image = "button_left";
      buttonOptions.hoverImage = "hover_button_left";
      buttonOptions.pressImage = "press_button_left";
      buttonOptions.onClick = [&]() { mSystemControls->speed /= 2.0; };
      Button* slower = playback->Add<Button>(buttonOptions);
      slower->ConstrainHeightTo(playback);
      slower->ConstrainTopAlignedTo(playback);

      buttonOptions.image = "button_right";
      buttonOptions.hoverImage = "hover_button_right";
      buttonOptions.pressImage = "press_button_right";
      buttonOptions.onClick = [&]() { mSystemControls->speed *= 2.0; };
      Button* faster = playback->Add<Button>(buttonOptions);
      faster->ConstrainHeightTo(playback);
      faster->ConstrainTopAlignedTo(playback);
   }

   // Bone information
   UIElement* boneHeader = Add<UIStackView>();
   boneHeader->ConstrainToRightOf(dockStateInfo, 64);
   boneHeader->ConstrainTopAlignedTo(dockStateInfo);
   {
      UIStackView* row1 = boneHeader->Add<UIStackView>();
      row1->SetVertical(false);
      row1->SetOffset(8.0);
      row1->ConstrainHeight(19);
      row1->ConstrainLeftAlignedTo(boneHeader);

      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");
      buttonOptions.image = "button_left";
      buttonOptions.hoverImage = "hover_button_left";
      buttonOptions.pressImage = "press_button_left";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<PrevBoneCommand>(this); };
      Button* prevBoneButton = row1->Add<Button>(buttonOptions);
      prevBoneButton->ConstrainTopAlignedTo(row1);
      prevBoneButton->ConstrainHeightTo(row1);

      mBoneName = row1->Add<Text>(Text::Options{"Bone name"});
      mBoneName->ConstrainTopAlignedTo(row1);
      mBoneName->ConstrainHeightTo(row1);

      buttonOptions.image = "button_right";
      buttonOptions.hoverImage = "hover_button_right";
      buttonOptions.pressImage = "press_button_right";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<NextBoneCommand>(this); };
      Button* nextBoneButton = row1->Add<Button>(buttonOptions);
      nextBoneButton->ConstrainTopAlignedTo(row1);
      nextBoneButton->ConstrainHeightTo(row1);

      UIStackView* row2 = boneHeader->Add<UIStackView>();
      row2->SetVertical(false);
      row2->SetOffset(8.0);
      row2->ConstrainHeightTo(row1);
      row2->ConstrainLeftAlignedTo(row1);

      Text* parentLabel = row2->Add<Text>(Text::Options{"Parent"});
      parentLabel->ConstrainTopAlignedTo(row2);
      parentLabel->ConstrainHeightTo(row2);

      Text::Options parentOptions{"N/A"};
      parentOptions.size = 12;
      parentOptions.alignment = Engine::Graphics::Font::Right;
      mBoneParent = row2->Add<Text>(parentOptions);
      mBoneParent->ConstrainTopAlignedTo(row2);
      mBoneParent->ConstrainHeightTo(row2);

      buttonOptions.image = "button_up";
      buttonOptions.hoverImage = "hover_button_up";
      buttonOptions.pressImage = "press_button_up";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<ParentBoneCommand>(this); };
      Button* parentBoneButton = row2->Add<Button>(buttonOptions);
      parentBoneButton->ConstrainTopAlignedTo(row2);
      parentBoneButton->ConstrainHeightTo(row2);

      // parentBoneButton->ConstrainRightAlignedTo(nextBoneButton);
      // mBoneParent->ConstrainRightAlignedTo(parentBoneButton, 8);
   }

   // Bone positions, rotations and sliders
   UIElement* bonePosition = Add<UIElement>();
   UIElement* boneRotation = Add<UIElement>();
   bonePosition->ConstrainBelow(boneHeader, 8);
   bonePosition->ConstrainLeftAlignedTo(boneHeader);

   boneRotation->ConstrainTopAlignedTo(bonePosition);
   boneRotation->ConstrainRightAlignedTo(boneHeader);
   boneRotation->ConstrainToRightOf(bonePosition);
   boneRotation->ConstrainWidthTo(bonePosition);

   {
      // Position/rotation icons that look good
      Image* position = bonePosition->Add<Image>(Image::Options{Asset::Image("EditorIcons.png"), "position"});
      Image* rotation = boneRotation->Add<Image>(Image::Options{Asset::Image("EditorIcons.png"), "rotation"});

      position->ConstrainWidth(37);
      position->ConstrainLeftAlignedTo(bonePosition);
      position->ConstrainVerticalCenterTo(bonePosition);
      rotation->ConstrainWidth(40);
      rotation->ConstrainLeftAlignedTo(boneRotation);
      rotation->ConstrainVerticalCenterTo(boneRotation);

      // Bone position/rotation reset buttons
      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");
      buttonOptions.image = "reset";

      // Reset position
      buttonOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mController->time);
         if (mController->time == keyframe.time)
         {
            //CommandStack::Instance()->Do<ResetBoneCommand>(this, mSkeleton->bones[mBone].originalPosition, keyframe.rotations[mBone]);
         }
      };
      Button* resetPositionButton = bonePosition->Add<Button>(buttonOptions);
      resetPositionButton->ConstrainWidth(35);
      resetPositionButton->ConstrainLeftAlignedTo(position);
      resetPositionButton->ConstrainBelow(position, 8);

      // Set position to previous
      buttonOptions.onClick = [&]() {
         State& state = GetCurrentState();
         size_t index = GetKeyframeIndex(state, mController->time);
         Keyframe& keyframe = state.keyframes[index];
         if (mController->time == keyframe.time && index > 0)
         {
            //Keyframe& prev = state.keyframes[index - 1];
            //CommandStack::Instance()->Do<ResetBoneCommand>(this, prev.positions[mBone], keyframe.rotations[mBone]);
         }
      };
      Button* setPrevPositionButton = bonePosition->Add<Button>(buttonOptions);
      setPrevPositionButton->ConstrainWidth(35);
      setPrevPositionButton->ConstrainLeftAlignedTo(resetPositionButton);
      setPrevPositionButton->ConstrainBelow(resetPositionButton, 8);

      // Reset rotation
      buttonOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mController->time);
         if (mController->time == keyframe.time)
         {
            //CommandStack::Instance()->Do<ResetBoneCommand>(this, keyframe.positions[mBone], mSkeleton->bones[mBone].originalRotation);
         }
      };
      Button* resetRotationButton = boneRotation->Add<Button>(buttonOptions);
      resetRotationButton->ConstrainWidth(35);
      resetRotationButton->ConstrainLeftAlignedTo(rotation);
      resetRotationButton->ConstrainBelow(rotation, 8);

      // Set rotation to previous
      buttonOptions.onClick = [&]() {
         State& state = GetCurrentState();
         size_t index = GetKeyframeIndex(state, mController->time);
         Keyframe& keyframe = state.keyframes[index];
         if (mController->time == keyframe.time && index > 0)
         {
            //Keyframe& prev = state.keyframes[index - 1];
            //CommandStack::Instance()->Do<ResetBoneCommand>(this, keyframe.positions[mBone], prev.rotations[mBone]);
         }
      };
      Button* setPrevRotationButton = bonePosition->Add<Button>(buttonOptions);
      setPrevRotationButton->ConstrainWidth(35);
      setPrevRotationButton->ConstrainLeftAlignedTo(resetRotationButton);
      setPrevRotationButton->ConstrainBelow(resetRotationButton, 8);

      // Bone position/rotation controls
      NumDisplay<float>::Options textOptions;
      textOptions.text = "0.0";
      textOptions.precision = 1;

      Scrubber<float>::Options scrubberOptions;
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.onChange = [&](double, double) { mpRoot->Emit<SkeletonModifiedEvent>(mController); };
      scrubberOptions.sensitivity = 0.1;

      UIStackView* positionScrubbers = bonePosition->Add<UIStackView>("PositionScrubbers");
      UIStackView* rotationScrubbers = boneRotation->Add<UIStackView>("RotationScrubbers");
      positionScrubbers->ConstrainToRightOf(resetPositionButton, 32);
      positionScrubbers->ConstrainHeightTo(bonePosition);
      positionScrubbers->ConstrainTopAlignedTo(bonePosition);
      positionScrubbers->ConstrainRightAlignedTo(bonePosition, 12);
      rotationScrubbers->ConstrainToRightOf(resetRotationButton, 32);
      rotationScrubbers->ConstrainHeightTo(boneRotation);
      rotationScrubbers->ConstrainTopAlignedTo(boneRotation);
      rotationScrubbers->ConstrainRightAlignedTo(boneRotation, 12);

      for (int i = 0; i < 3; i++)
      {
         mBonePos[i].text = positionScrubbers->Add<NumDisplay<float>>(textOptions);
         mBonePos[i].scrubber = positionScrubbers->Add<Scrubber<float>>(scrubberOptions);
         mBoneRot[i].text = rotationScrubbers->Add<NumDisplay<float>>(textOptions);
         mBoneRot[i].scrubber = rotationScrubbers->Add<Scrubber<float>>(scrubberOptions);

         mBonePos[i].text->ConstrainHeight(32);
         mBonePos[i].text->ConstrainLeftAlignedTo(positionScrubbers);
         mBonePos[i].scrubber->ConstrainHeight(7);
         mBonePos[i].scrubber->ConstrainLeftAlignedTo(positionScrubbers);
         mBoneRot[i].text->ConstrainHeight(32);
         mBoneRot[i].text->ConstrainLeftAlignedTo(rotationScrubbers);
         mBoneRot[i].scrubber->ConstrainHeight(7);
         mBoneRot[i].scrubber->ConstrainLeftAlignedTo(rotationScrubbers);
      }

      mBonePos[0].scrubber->ConstrainRightAlignedTo(bonePosition);
      mBoneRot[0].scrubber->ConstrainRightAlignedTo(boneRotation);
   }

   for (size_t i = 0; i < 20; ++i)
   {
      AddKeyframeIcon();
   }

   root->Subscribe<SkeletonLoadedEvent>(*this);
   root->Subscribe<SkeletonSelectedEvent>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<AnimationController>>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<AnimationSystemController>>(*this);
}

///
///
///
void Dock::Receive(const SkeletonLoadedEvent& evt)
{
   mController = evt.component;
   SetState(0);
   SetTime(0);
   SetBone(0);
}

///
///
///
void Dock::Receive(const SkeletonSelectedEvent& evt)
{
   mSkeleton = evt.index;
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<AnimationController>& evt)
{
   mController = evt.component;
   mScrubber->Bind(&mController->time);
   mTime->Bind(&mController->time);
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<AnimationSystemController>& evt)
{
   mSystemControls = evt.component;
}

///
///
///
State& Dock::GetCurrentState()
{
   return mController->GetCurrentState();
}

///
///
///
Keyframe& Dock::GetCurrentKeyframe()
{
   return GetKeyframe(GetCurrentState(), mController->time);
}

///
///
///
void Dock::Update(TIMEDELTA dt)
{
   // Update the play/pause buttons
   mPlay->SetActive(mSystemControls->paused);
   mPause->SetActive(!mSystemControls->paused);

   UIElement::Update(dt);
}

///
///
///
void Dock::AddKeyframeIcon()
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
      fImage.left == fKeyframes.left + kTimelineWidth * entry.second - (fImage.right - fImage.left) / 2,
   });

   mKeyframeIcons.push_back(entry);
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
      AddKeyframeIcon();
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
   mController->current = index;

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
void Dock::SetBone(const AnimationController::BoneID& boneId)
{
   mBone = boneId;

   // Update bone info
   Bone* bone = mController->GetBone(mBone);
   Bone* parent = mController->GetBone(mController->ParentBone(mBone));
   mBoneName->SetText(bone->name);
   mBoneParent->SetText(parent->name);

   mBonePos[0].text->Bind(&bone->position.x);
   mBonePos[1].text->Bind(&bone->position.y);
   mBonePos[2].text->Bind(&bone->position.z);
   mBoneRot[0].text->Bind(&bone->rotation.x);
   mBoneRot[1].text->Bind(&bone->rotation.y);
   mBoneRot[2].text->Bind(&bone->rotation.z);

   State& state = GetCurrentState();
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mController->GetSkeletonForBone(mBone);
   if (mSelectedKeyframe <= state.keyframes.size())
   {
      mBonePos[0].scrubber->Bind(&state.keyframes[mSelectedKeyframe].positions[mBone].x);
      mBonePos[1].scrubber->Bind(&state.keyframes[mSelectedKeyframe].positions[mBone].y);
      mBonePos[2].scrubber->Bind(&state.keyframes[mSelectedKeyframe].positions[mBone].z);
      mBoneRot[0].scrubber->Bind(&state.keyframes[mSelectedKeyframe].rotations[mBone].x);
      mBoneRot[1].scrubber->Bind(&state.keyframes[mSelectedKeyframe].rotations[mBone].y);
      mBoneRot[2].scrubber->Bind(&state.keyframes[mSelectedKeyframe].rotations[mBone].z);
   }
}

///
///
///
void Dock::AddStateCommand::Do()
{
   if (state.keyframes.size() == 0)
   {
      Keyframe keyframe;
      for (size_t i = 0; i < dock->mController->NumSkeletons(); i++)
      {
         Engine::ComponentHandle<AnimatedSkeleton> skeleton = dock->mController->GetSkeleton(i);
         std::vector<Bone>& bones = skeleton->bones;
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.positions), [](const Bone& b) { return b.originalPosition; });
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.rotations), [](const Bone& b) { return b.originalRotation; });
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.scales), [](const Bone& b) { return b.originalScale; });
      }

      keyframe.time = 0;
      state.keyframes.push_back(keyframe);

      state.skeletonId = dock->mSkeleton;
   }

   dock->mController->states.insert(dock->mController->states.begin() + dock->mController->current + (afterCurrent ? 1 : 0), state);
   if (afterCurrent)
   {
      dock->SetState(dock->mController->current + 1);
   }
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::AddStateCommand::Undo()
{
   afterCurrent = dock->mController->current > 0;
   // Get current state as a copy not a reference
   state = dock->GetCurrentState();

   dock->mController->states.erase(dock->mController->states.begin() + dock->mController->current);
   if (afterCurrent)
   {
      dock->SetState(dock->mController->current - 1);
   }
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
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
   SetTime(mController->time * stretch);
   mpRoot->Emit<SkeletonModifiedEvent>(mController);
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
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::AddKeyframeCommand::Do()
{
   State& state = dock->GetCurrentState();

   if (keyframe.positions.size() != dock->mController->bones.size())
   {
      keyframe.positions.clear();
      keyframe.rotations.clear();

      // Insert the CURRENT values of position and rotation at this timestamp.
      for (size_t i = 0; i < dock->mController->NumSkeletons(); i++)
      {
         Engine::ComponentHandle<AnimatedSkeleton> skeleton = dock->mController->GetSkeleton(i);

         std::vector<Bone>& bones = skeleton->bones;
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.positions), [](const Bone& b) { return b.position; });
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.rotations), [](const Bone& b) { return b.rotation; });
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.scales), [](const Bone& b) { return b.scale; });
      }
   }

   keyframeIndex = GetKeyframeIndex(state, dock->mController->time) + 1;
   state.keyframes.insert(state.keyframes.begin() + keyframeIndex, keyframe);
   dock->UpdateKeyframeIcons();
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
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
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::SetTime(double time)
{
   State& state = GetCurrentState();

   size_t keyframeIndex = GetKeyframeIndex(state, time);
   Keyframe& prev = state.keyframes[keyframeIndex];
   mSelectedKeyframe = std::numeric_limits<size_t>::max();

   if ((time - prev.time) / state.length < 0.02)
   {
      mController->time = prev.time;
      mSelectedKeyframe = keyframeIndex;
   }
   else if (
      (keyframeIndex < state.keyframes.size() - 1) && 
      (state.keyframes[keyframeIndex + 1].time - time) / state.length < 0.02
   )
   {
      mController->time = state.keyframes[keyframeIndex + 1].time;
      mSelectedKeyframe = keyframeIndex + 1;
   }
   else if ((state.length - time) / state.length < 0.02)
   {
      mController->time = state.length;
   }
   else
   {
      mController->time = time;
   }

   if (mSelectedKeyframe >= state.keyframes.size())
   {
      mKeyframeTime->Bind(nullptr);
      mBonePos[0].scrubber->Bind(nullptr);
      mBonePos[1].scrubber->Bind(nullptr);
      mBonePos[2].scrubber->Bind(nullptr);
      mBoneRot[0].scrubber->Bind(nullptr);
      mBoneRot[1].scrubber->Bind(nullptr);
      mBoneRot[2].scrubber->Bind(nullptr);
   }
   else
   {
      mKeyframeTime->Bind(&state.keyframes[mSelectedKeyframe].time);
      mBonePos[0].scrubber->Bind(&state.keyframes[mSelectedKeyframe].positions[mBone].x);
      mBonePos[1].scrubber->Bind(&state.keyframes[mSelectedKeyframe].positions[mBone].y);
      mBonePos[2].scrubber->Bind(&state.keyframes[mSelectedKeyframe].positions[mBone].z);
      mBoneRot[0].scrubber->Bind(&state.keyframes[mSelectedKeyframe].rotations[mBone].x);
      mBoneRot[1].scrubber->Bind(&state.keyframes[mSelectedKeyframe].rotations[mBone].y);
      mBoneRot[2].scrubber->Bind(&state.keyframes[mSelectedKeyframe].rotations[mBone].z);
   }
}

///
///
///
void Dock::NextStateCommand::Do()
{
   if (dock->mController->current >= dock->mController->states.size() - 1)
   {
      dock->SetState(0);
   }
   else
   {
      dock->SetState(dock->mController->current + 1);
   }
}

///
///
///
void Dock::NextStateCommand::Undo()
{
   if (dock->mController->current == 0)
   {
      dock->SetState(dock->mController->states.size() - 1);
   }
   else
   {
      dock->SetState(dock->mController->current - 1);
   }
}

///
///
///
void Dock::SetKeyframeTimeCommand::Do()
{
   State& state = dock->GetCurrentState();
   Keyframe& keyframe = GetKeyframe(state, dock->mController->time);
   if (dock->mController->time != keyframe.time)
   {
      return;
   }

   double last = keyframe.time;
   dock->mController->time = keyframe.time = value;
   dock->UpdateKeyframeIcons();
   value = last;

   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::NextBoneCommand::Do()
{
   dock->SetBone(dock->mController->NextBone(dock->mBone));
}

///
///
///
void Dock::NextBoneCommand::Undo()
{
   dock->SetBone(dock->mController->PrevBone(dock->mBone));
}

///
///
///
void Dock::ParentBoneCommand::Do()
{
   last = dock->mBone;
   dock->SetBone(dock->mController->ParentBone(dock->mBone));
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
   Keyframe& keyframe = GetKeyframe(state, dock->mController->time);
   if (dock->mController->time != keyframe.time)
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

   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

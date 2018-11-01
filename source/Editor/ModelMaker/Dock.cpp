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

namespace ModelMaker
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
   UIFrame& fCursor = mScrubber->GetFrame();
   root->AddConstraints({
      fCursor.left == fTimeline.left,
      fCursor.top == fTimeline.bottom - 8,
      fCursor.width == fTimeline.width,
      fCursor.height == 10,
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
         fKeyframe.top == fTimeline.bottom - 32,
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
      });

      buttonOptions.image = "button_left";
      buttonOptions.hoverImage = "hover_button_left";
      buttonOptions.pressImage = "press_button_left";
      buttonOptions.onClick = [&]() {
         mController->speed /= 2.0;
      };
      UIFrame& fSlower = Add<Button>(buttonOptions)->GetFrame();

      buttonOptions.image = "button_right";
      buttonOptions.hoverImage = "hover_button_right";
      buttonOptions.pressImage = "press_button_right";
      buttonOptions.onClick = [&]() {
         mController->speed *= 2.0;
      };
      UIFrame& fFaster = Add<Button>(buttonOptions)->GetFrame();

      root->AddConstraints({
         fSlower.left == fPlay.left,
         fSlower.top == fPlay.bottom - 8,
         fSlower.height == 19,
         fFaster.left == fSlower.right + 8,
         fFaster.top == fSlower.top,
         fFaster.height == fSlower.height,
      });
   }

   // Bone information
   UIElement* boneHeader = Add<UIElement>();
   UIFrame& fBoneHeader = boneHeader->GetFrame();
   {
      mBoneName = boneHeader->Add<Text>(Text::Options{"Bone name"});
      UIFrame& fBoneName = mBoneName->GetFrame();

      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");
      buttonOptions.image = "button_left";
      buttonOptions.hoverImage = "hover_button_left";
      buttonOptions.pressImage = "press_button_left";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<PrevBoneCommand>(this); };
      UIFrame& fPrevBone = boneHeader->Add<Button>(buttonOptions)->GetFrame();

      buttonOptions.image = "button_right";
      buttonOptions.hoverImage = "hover_button_right";
      buttonOptions.pressImage = "press_button_right";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<NextBoneCommand>(this); };
      UIFrame& fNextBone = boneHeader->Add<Button>(buttonOptions)->GetFrame();

      UIFrame& fParentLabel = boneHeader->Add<Text>(Text::Options{"Parent"})->GetFrame();
      Text::Options parentOptions{"N/A"};
      parentOptions.size = 12;
      parentOptions.alignment = Engine::Graphics::Font::Right;
      mBoneParent = boneHeader->Add<Text>(parentOptions);
      UIFrame& fParentValue = mBoneParent->GetFrame();

      buttonOptions.image = "button_up";
      buttonOptions.hoverImage = "hover_button_up";
      buttonOptions.pressImage = "press_button_up";
      buttonOptions.onClick = [&]() { CommandStack::Instance()->Do<ParentBoneCommand>(this); };
      UIFrame& fParentBone = Add<Button>(buttonOptions)->GetFrame();

      root->AddConstraints({
         fBoneHeader.left == fTimeline.right + 48,
         fBoneHeader.top == fStateName.top,
         fBoneHeader.bottom == fParentBone.bottom,
         fBoneHeader.width >= 200,

         fPrevBone.top == fBoneHeader.top,
         fPrevBone.left == fBoneHeader.left,
         fPrevBone.height == 19,
         fBoneName.top == fPrevBone.top,
         fBoneName.bottom == fPrevBone.bottom,
         fBoneName.left == fPrevBone.right + 8,
         fBoneName.right == fNextBone.left - 8,
         fNextBone.top == fPrevBone.top,
         fNextBone.right == fBoneHeader.right,
         fNextBone.height == fPrevBone.height,

         fParentLabel.top == fPrevBone.bottom - 16,
         fParentLabel.left == fBoneHeader.left,
         fParentLabel.height == 19,
         fParentValue.top == fParentLabel.top,
         fParentValue.left >= fParentLabel.right + 8,
         fParentValue.right == fParentBone.left - 8,
         fParentValue.height == fParentLabel.height,
         fParentBone.top == fParentValue.top,
         fParentBone.right == fBoneHeader.right,
         fParentBone.height == fParentValue.height,
         fParentBone.width == 19,
      });
   }

   // Bone positions, rotations and sliders
   UIElement* bonePosition = Add<UIElement>();
   UIFrame& fBonePosition = bonePosition->GetFrame();
   UIElement* boneRotation = Add<UIElement>();
   UIFrame& fBoneRotation = boneRotation->GetFrame();
   root->AddConstraints({
      fBonePosition.top == fBoneHeader.bottom - 8,
      fBonePosition.left == fBoneHeader.left,
      fBoneRotation.top == fBonePosition.top,
      fBoneRotation.right == fBoneHeader.right,

      // Split the bone section in half
      fBonePosition.right == fBoneRotation.left,
      fBonePosition.width == fBoneRotation.width,
   });

   {
      // Position/rotation icons that look good
      Image::Options imageOptions;
      imageOptions.filename = Asset::Image("EditorIcons.png");
      imageOptions.image = "position";
      UIFrame& fPosition = bonePosition->Add<Image>(imageOptions)->GetFrame();
      imageOptions.image = "rotation";
      UIFrame& fRotation = boneRotation->Add<Image>(imageOptions)->GetFrame();

      root->AddConstraints({
         fPosition.width == 37,
         fPosition.left == fBonePosition.left,
         fPosition.centerY == fBonePosition.centerY,

         fRotation.width == 40,
         fRotation.left == fBoneRotation.left,
         fRotation.centerY == fBoneRotation.centerY,
      });

      // Bone position/rotation reset buttons
      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");
      buttonOptions.image = "reset";
      buttonOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mSkeleton->time);
         if (mSkeleton->time == keyframe.time)
         {
            CommandStack::Instance()->Do<ResetBoneCommand>(this, mSkeleton->bones[mBone].originalPosition, keyframe.rotations[mBone]);
         }
      };
      UIFrame& fResetPosition = bonePosition->Add<Button>(buttonOptions)->GetFrame();

      buttonOptions.onClick = [&]() {
         Keyframe& keyframe = GetKeyframe(GetCurrentState(), mSkeleton->time);
         if (mSkeleton->time == keyframe.time)
         {
            CommandStack::Instance()->Do<ResetBoneCommand>(this, keyframe.positions[mBone], mSkeleton->bones[mBone].originalRotation);
         }
      };
      UIFrame& fResetRotation = boneRotation->Add<Button>(buttonOptions)->GetFrame();

      root->AddConstraints({
         fResetPosition.width == 35,
         fResetPosition.left == fBonePosition.left,
         fResetPosition.top == fPosition.bottom - 8,

         fResetRotation.width == 35,
         fResetRotation.left == fBoneRotation.left,
         fResetRotation.top == fRotation.bottom - 8,
      });

      // Bone position/rotation controls
      NumDisplay<float>::Options textOptions;
      textOptions.text = "0.0";
      textOptions.precision = 1;

      Scrubber<float>::Options scrubberOptions;
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.onChange = [&](double, double) { mpRoot->Emit<ModelModifiedEvent>(mSkeleton); };
      scrubberOptions.sensitivity = 0.1;

      for (int i = 0; i < 3; i++)
      {
         mBonePos[i].text = bonePosition->Add<NumDisplay<float>>(textOptions);
         mBonePos[i].scrubber = bonePosition->Add<Scrubber<float>>(scrubberOptions);
         mBoneRot[i].text = boneRotation->Add<NumDisplay<float>>(textOptions);
         mBoneRot[i].scrubber = boneRotation->Add<Scrubber<float>>(scrubberOptions);
         UIFrame& bonePosText = mBonePos[i].text->GetFrame();
         UIFrame& bonePosValue = mBonePos[i].scrubber->GetFrame();
         UIFrame& boneRotText = mBoneRot[i].text->GetFrame();
         UIFrame& boneRotValue = mBoneRot[i].scrubber->GetFrame();

         root->AddConstraints({
            bonePosText.left == fResetPosition.right + 8,
            bonePosText.height == 32,
            bonePosValue.top == bonePosText.bottom - 8,
            bonePosValue.left == bonePosText.left,
            bonePosValue.height == 7,

            boneRotText.left == fResetRotation.right + 8,
            boneRotText.height == 32,
            boneRotValue.top == boneRotText.bottom - 8,
            boneRotValue.left == boneRotText.left,
            boneRotValue.height == 7,
         });

         if (i == 0)
         {
            root->AddConstraints({
               bonePosText.top == fBonePosition.top,
               boneRotText.top == fBoneRotation.top,
            });
         }
         else
         {
            root->AddConstraints({
               bonePosText.top == mBonePos[i - 1].scrubber->GetFrame().bottom - 8,
               boneRotText.top == mBoneRot[i - 1].scrubber->GetFrame().bottom - 8,
            });
         }
      }
   }

   root->Subscribe<ModelLoadedEvent>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<AnimatedSkeleton>>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<AnimationSystemController>>(*this);
}

///
///
///
void Dock::Receive(const ModelLoadedEvent& evt)
{
   mSkeleton = evt.component;
   SetState(0);
   SetTime(0);
   SetBone(0);
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

   mBonePos[0].text->Bind(&bone.position.x);
   mBonePos[1].text->Bind(&bone.position.y);
   mBonePos[2].text->Bind(&bone.position.z);
   mBoneRot[0].text->Bind(&bone.rotation.x);
   mBoneRot[1].text->Bind(&bone.rotation.y);
   mBoneRot[2].text->Bind(&bone.rotation.z);

   State& state = GetCurrentState();
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
      std::vector<Bone>& bones = dock->mSkeleton->bones;
      std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.positions), [](const Bone& b) { return b.originalPosition; });
      std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.rotations), [](const Bone& b) { return b.originalRotation; });

      keyframe.time = 0;
      state.keyframes.push_back(keyframe);
   }

   dock->mSkeleton->states.insert(dock->mSkeleton->states.begin() + dock->mSkeleton->current + (afterCurrent ? 1 : 0), state);
   if (afterCurrent)
   {
      dock->SetState(dock->mSkeleton->current + 1);
   }
   dock->mpRoot->Emit<ModelModifiedEvent>(dock->mSkeleton);
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
   dock->mpRoot->Emit<ModelModifiedEvent>(dock->mSkeleton);
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
   mpRoot->Emit<ModelModifiedEvent>(mSkeleton);
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
   dock->mpRoot->Emit<ModelModifiedEvent>(dock->mSkeleton);
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
   dock->mpRoot->Emit<ModelModifiedEvent>(dock->mSkeleton);
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
   dock->mpRoot->Emit<ModelModifiedEvent>(dock->mSkeleton);
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

   dock->mpRoot->Emit<ModelModifiedEvent>(dock->mSkeleton);
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

   dock->mpRoot->Emit<ModelModifiedEvent>(dock->mSkeleton);
}

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld

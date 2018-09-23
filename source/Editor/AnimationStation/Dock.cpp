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

///
///
///
std::pair<glm::vec3, glm::vec3> GetLocalPositionAndRotation(const AnimatedSkeleton& skeleton, size_t boneId)
{
   const Bone& bone = skeleton.bones[boneId];
   glm::mat4 localMatrix = bone.matrix;
   if (boneId > 0)
   {
      // Is this too expensive to do every frame? Who knows! Wheeee! It's probably fine!
      glm::mat4 parentMatrixInv = glm::inverse(skeleton.bones[bone.parent].matrix);
      localMatrix = glm::inverse(skeleton.bones[bone.parent].matrix) * localMatrix;
   }
   glm::vec3 scale;
   glm::vec3 position;
   glm::quat rotation;
   glm::vec3 skew;
   glm::vec4 perspective;
   glm::decompose(localMatrix, scale, rotation, position, skew, perspective);

   glm::vec3 localPos(localMatrix[3][0], localMatrix[3][1], localMatrix[3][2]);

   // roll (x-axis rotation)
   float sinr_cosp = +2.0f * (rotation.w * rotation.x + rotation.y * rotation.z);
   float cosr_cosp = +1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y);
   float roll = atan2(sinr_cosp, cosr_cosp);

   // pitch (y-axis rotation)
   float sinp = +2.0f * (rotation.w * rotation.y - rotation.z * rotation.x);
   float pitch;
   if (fabs(sinp) >= 1)
      pitch = copysign((float)M_PI / 2.0f, sinp); // use 90 degrees if out of range
   else
      pitch = asin(sinp);

   // yaw (z-axis rotation)
   float siny_cosp = +2.0f * (rotation.w * rotation.z + rotation.x * rotation.y);
   float cosy_cosp = +1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z);
   float yaw = atan2(siny_cosp, cosy_cosp);

   return std::make_pair(position, glm::vec3(DEGREES(roll), DEGREES(pitch), DEGREES(yaw)));
}

///
///
///
void RecomputeBoneMatrix(State& state, Keyframe& keyframe, std::vector<Bone>& bones, size_t bone) {
   if (bone == 0)
   {
      keyframe.matrixes[bone] = glm::mat4(1);
   }
   else
   {
      keyframe.matrixes[bone] = keyframe.matrixes[bones[bone].parent];
   }

   glm::vec3 position = keyframe.positions[bone];
   glm::vec3 rotation = keyframe.rotations[bone];
   keyframe.matrixes[bone] = glm::translate(keyframe.matrixes[bone], position);
   keyframe.matrixes[bone] = glm::rotate(keyframe.matrixes[bone], RADIANS(rotation.y), glm::vec3(0, 1, 0));
   keyframe.matrixes[bone] = glm::rotate(keyframe.matrixes[bone], RADIANS(rotation.x), glm::vec3(1, 0, 0));
   keyframe.matrixes[bone] = glm::rotate(keyframe.matrixes[bone], RADIANS(rotation.z), glm::vec3(0, 0, 1));

   if (keyframe.time == 0)
   {
      Keyframe& tail = state.keyframes[state.keyframes.size() - 1];
      tail.positions[bone] = keyframe.positions[bone];
      tail.rotations[bone] = keyframe.rotations[bone];
      tail.matrixes[bone] = keyframe.matrixes[bone];
   }

   for (size_t child : bones[bone].children)
   {
      RecomputeBoneMatrix(state, keyframe, bones, child);
   }
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
      labelOptions.text = "Walk";
      labelOptions.onChange = [&](std::string value) { CommandStack::Instance()->Do<SetStateNameCommand>(this, value); };
      mStateName = Add<Label>(labelOptions);

      labelOptions.y -= 5 * EIGHT_Y;
      labelOptions.text = "0.0";
      labelOptions.onChange = nullptr;
      mStateLength = Add<Label>(labelOptions);

      labelOptions.y -= 15 * EIGHT_Y;
      labelOptions.text = "1";
      // mKeyframeIndex = Add<Label>(labelOptions);

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
      
      /*
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
      */

      imageOptions.w = 19.0f / GetWidth();
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
         Either<size_t, void> snapped = SetTime(mSkeleton->time);
         if (snapped.IsLeft())
         {
            CommandStack::Instance()->Do<RemoveKeyframeCommand>(this, snapped.Left());
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
      scrubberOptions.onRelease = std::bind(&Dock::FinishScrubbing, this, std::placeholders::_1);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::KEYFRAME_TIME, std::placeholders::_1);
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
      mTick->SetActive(false);
   }

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
      scrubberOptions.x = labelOptions.x + 2 * EIGHT_X;
      scrubberOptions.y = labelOptions.y - 2 * EIGHT_Y;
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

      scrubberOptions.y = labelOptions.y - 2 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::POS_Y);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::POS_Y, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.y -= 7 * EIGHT_Y;
      mBonePosZ = Add<Label>(labelOptions);

      scrubberOptions.y = labelOptions.y - 2 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::POS_Z);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::POS_Z, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.x += 18 * EIGHT_X;
      labelOptions.y = 1.0f - 15 * EIGHT_Y;
      mBoneRotX = Add<Label>(labelOptions);

      scrubberOptions.x = labelOptions.x + 2 * EIGHT_X;
      scrubberOptions.y = labelOptions.y - 2 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::ROT_X);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::ROT_X, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.y -= 7 * EIGHT_Y;
      mBoneRotY = Add<Label>(labelOptions);

      scrubberOptions.y = labelOptions.y - 2 * EIGHT_Y;
      scrubberOptions.onPress = std::bind(&Dock::StartScrubbing, this, ScrubType::ROT_Y);
      scrubberOptions.onMove = std::bind(&Dock::Scrub, this, ScrubType::ROT_Y, std::placeholders::_1);
      Add<StationaryScrubber>(scrubberOptions);

      labelOptions.y -= 7 * EIGHT_Y;
      mBoneRotZ = Add<Label>(labelOptions);

      scrubberOptions.y = labelOptions.y - 2 * EIGHT_Y;
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
      imageOptions.onClick = std::bind(&Dock::ResetPosition, this);
      Add<Image>(imageOptions);

      imageOptions.x += 18 * EIGHT_X;
      imageOptions.onClick = std::bind(&Dock::ResetRotation, this);
      Add<Image>(imageOptions);
   }

   Subscribe<Engine::ComponentAddedEvent<Game::AnimatedSkeleton>>(*this);
   Subscribe<Engine::ComponentAddedEvent<AnimationSystemController>>(*this);
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
   State& state = mSkeleton->states[mSkeleton->current];

   // Update the UI according to the current state of the skeleton
   mScrubber->SetValue(mSkeleton->time / state.length);
   mTime->SetText(Format::FormatString("%.2f", mSkeleton->time));

   // Add all but the ends of the animation as visible keyframes.
   while (mKeyframeIcons.size() < state.keyframes.size() - 1)
   {
      Image::Options keyframeOptions;
      keyframeOptions.x = 19.0f / GetWidth();
      keyframeOptions.y = 1.0f - 136.0f / GetHeight();
      keyframeOptions.w = 10.0f / GetWidth();
      keyframeOptions.h = 32.0f / GetHeight();
      keyframeOptions.filename = Asset::Image("EditorIcons.png");
      keyframeOptions.image = "keyframe";
      mKeyframeIcons.push_back(Add<Image>(keyframeOptions));
   }
   while (mKeyframeIcons.size() > state.keyframes.size() - 1)
   {
      Image* toRemove = mKeyframeIcons.back();
      mKeyframeIcons.pop_back();
      RemoveChild(toRemove);
   }
   for (size_t i = 0; i < mKeyframeIcons.size(); i++)
   {
      mKeyframeIcons[i]->SetOffset(glm::vec3(state.keyframes[i].time / state.length, 0, 0));
   }

   // Update bone info
   Bone& bone = mSkeleton->bones[mBone];
   {
      mBoneName->SetText(bone.name);

      if (mBone == 0)
      {
         mBoneParent->SetText(GetCurrentState().name);
      }
      else
      {
         mBoneParent->SetText("root");
      }

      glm::mat4 localMatrix = bone.matrix;
      // Is this too expensive to do every frame? Who knows! Wheeee! It's probably fine!
      std::pair<glm::vec3, glm::vec3> posAndRot = GetLocalPositionAndRotation(*mSkeleton, mBone);
      glm::vec3 position = posAndRot.first;
      glm::vec3 rotation = posAndRot.second;

      if (std::abs(position.x) < 0.1) { position.x = 0; }
      if (std::abs(position.y) < 0.1) { position.y = 0; }
      if (std::abs(position.z) < 0.1) { position.z = 0; }
      if (std::abs(rotation.x) < 0.1) { rotation.x = 0; }
      if (std::abs(rotation.y) < 0.1) { rotation.y = 0; }
      if (std::abs(rotation.z) < 0.1) { rotation.z = 0; }

      mBonePosX->SetText(Format::FormatString("%.1f", position.x));
      mBonePosY->SetText(Format::FormatString("%.1f", position.y));
      mBonePosZ->SetText(Format::FormatString("%.1f", position.z));
      mBoneRotX->SetText(Format::FormatString("%.1f", rotation.x));
      mBoneRotY->SetText(Format::FormatString("%.1f", rotation.y));
      mBoneRotZ->SetText(Format::FormatString("%.1f", rotation.z));
   }

   mPlay->SetActive(mController->paused);
   mTick->SetActive(mController->paused);
   mPause->SetActive(!mController->paused);

   SubWindow::Update(dt);
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
}

///
///
///
void Dock::NextState()
{
   if (mSkeleton->current >= mSkeleton->states.size() - 1)
   {
      SetState(0);
   }
   else
   {
      SetState(mSkeleton->current + 1);
   }
}

///
///
///
void Dock::PrevState()
{
   if (mSkeleton->current == 0)
   {
      SetState(mSkeleton->states.size() - 1);
   }
   else
   {
      SetState(mSkeleton->current - 1);
   }
}

///
///
///
void Dock::AddState(State state, bool afterCurrent)
{
   if (state.keyframes.size() == 0)
   {
      Keyframe keyframe;
      std::transform(mSkeleton->bones.begin(), mSkeleton->bones.end(), std::back_inserter(keyframe.positions), [](const Bone& b) { return b.position; });
      std::transform(mSkeleton->bones.begin(), mSkeleton->bones.end(), std::back_inserter(keyframe.rotations), [](const Bone& b) { return b.rotation; });
      std::transform(mSkeleton->bones.begin(), mSkeleton->bones.end(), std::back_inserter(keyframe.matrixes), [](const Bone& b) { return b.matrix; });

      for (size_t boneId = 0; boneId < mSkeleton->bones.size(); boneId++)
      {
         RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, boneId);
      }

      keyframe.time = 0;
      state.keyframes.push_back(keyframe);

      keyframe.time = state.length;
      state.keyframes.push_back(keyframe);
   }

   mSkeleton->states.insert(mSkeleton->states.begin() + mSkeleton->current + (afterCurrent ? 1 : 0), state);
   if (afterCurrent)
   {
      SetState(mSkeleton->current + 1);
   }
}

///
///
///
State Dock::RemoveState()
{
   // GetCurrentState() as a copy not a reference
   State state = GetCurrentState();
   mSkeleton->states.erase(mSkeleton->states.begin() + mSkeleton->current);
   if (mSkeleton->current > 0)
   {
      SetState(mSkeleton->current - 1);
   }
   return state;
}

///
///
///
void Dock::ChangeStateLength(double increment)
{
   State& state = GetCurrentState();

   double stretch = (state.length + increment) / state.length;
   state.length += increment;

   for (Keyframe& keyframe : state.keyframes)
   {
      keyframe.time *= stretch;
   }
   mSkeleton->time *= stretch;
}

///
///
///
void Dock::SetStateName(std::string name)
{
   State& state = GetCurrentState();
   state.name = name;
   mStateName->SetText(state.name);
}

///
///
///
size_t Dock::AddKeyframe(Keyframe keyframe)
{
   State& state = GetCurrentState();

   if (keyframe.matrixes.size() == 0)
   {
      std::transform(mSkeleton->bones.begin(), mSkeleton->bones.end(), std::back_inserter(keyframe.matrixes), [](const Bone& b) { return b.matrix; });
      for (size_t boneId = 0; boneId < mSkeleton->bones.size(); boneId++)
      {
         auto posAndRot = GetLocalPositionAndRotation(*mSkeleton, boneId);
         keyframe.positions.push_back(posAndRot.first);
         keyframe.rotations.push_back(posAndRot.second);
      }
   }

   size_t keyframeIndex = GetKeyframeIndex(state, mSkeleton->time);
   state.keyframes.insert(state.keyframes.begin() + keyframeIndex + 1, keyframe);
   return keyframeIndex + 1;
}

///
///
///
Keyframe Dock::RemoveKeyframe(size_t index)
{
   State& state = GetCurrentState();

   Keyframe keyframe = state.keyframes[index];
   state.keyframes.erase(state.keyframes.begin() + index);
   return keyframe;
}

///
///
///
Either<size_t, void> Dock::SetTime(double time)
{
   State& state = GetCurrentState();

   size_t keyframeIndex = GetKeyframeIndex(state, time);
   Keyframe& prev = state.keyframes[keyframeIndex];

   if ((time - prev.time) / state.length < 0.02)
   {
      mSkeleton->time = prev.time;
      return keyframeIndex;
   }
   else if (
      (keyframeIndex < state.keyframes.size() - 2) && 
      (state.keyframes[keyframeIndex + 1].time - time) / state.length < 0.02)
   {
      mSkeleton->time = state.keyframes[keyframeIndex + 1].time;
      return keyframeIndex + 1;
   }
   else
   {
      mSkeleton->time = time;
      return nullptr;
   }
}

///
///
///
void Dock::NextBone()
{
   if (++mBone >= mSkeleton->bones.size())
   {
      mBone = 0;
   }
}

///
///
///
void Dock::PrevBone()
{
   if (++mBone >= mSkeleton->bones.size())
   {
      mBone = 0;
   }
}

///
///
///
void Dock::ParentBone()
{
   if (mBone > 0)
   {
      mBone = mSkeleton->bones[mBone].parent;
   }
}

///
///
///
void Dock::StartScrubbing(ScrubType type)
{
   State& state = GetCurrentState();

   Keyframe& keyframe = GetKeyframe(state, mSkeleton->time);
   if (mSkeleton->time != keyframe.time)
   {
      return;
   }

   switch (type)
   {
   case KEYFRAME_TIME:
      mScrubbing = std::make_unique<SetKeyframeTimeCommand>(this, keyframe.time);
      break;
   case POS_X:
   case POS_Y:
   case POS_Z:
   case ROT_X:
   case ROT_Y:
   case ROT_Z:
      mScrubbing = std::make_unique<SetBoneCommand>(this, keyframe.positions[mBone], keyframe.rotations[mBone]);
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
   mScrubbing->Do();
   CommandStack::Instance()->Do(std::move(mScrubbing));
}

///
///
///
void Dock::Scrub(ScrubType type, double amount)
{
   State& state = GetCurrentState();

   size_t index = GetKeyframeIndex(state, mSkeleton->time);
   Keyframe& keyframe = state.keyframes[index];
   if (mSkeleton->time != keyframe.time)
   {
      return;
   }

   switch (type)
   {
   case KEYFRAME_TIME:
      keyframe.time += amount * state.length / 4.0f;

      if (index == 0)
      {
         keyframe.time = 0;
      }
      else if (index == state.keyframes.size() - 1)
      {
         keyframe.time = state.length;
      }
      else if (keyframe.time <= state.keyframes[index - 1].time + state.length * 0.01f)
      {
         keyframe.time = state.keyframes[index - 1].time + state.length * 0.01f;
      }
      else if (keyframe.time >= state.keyframes[index + 1].time - state.length * 0.01f)
      {
         keyframe.time = state.keyframes[index + 1].time - state.length * 0.01f;
      }

      mSkeleton->time = keyframe.time;
      break;
   case POS_X:
      keyframe.positions[mBone].x += (float)amount;
      RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, mBone);
      break;
   case POS_Y:
      keyframe.positions[mBone].y += (float)amount;
      RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, mBone);
      break;
   case POS_Z:
      keyframe.positions[mBone].z += (float)amount;
      RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, mBone);
      break;
   case ROT_X:
      keyframe.rotations[mBone].x += 10 * (float)amount;
      RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, mBone);
      break;
   case ROT_Y:
      keyframe.rotations[mBone].y += 10 * (float)amount;
      RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, mBone);
      break;
   case ROT_Z:
      keyframe.rotations[mBone].z += 10 * (float)amount;
      RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, mBone);
      break;
   }
}

///
///
///
void Dock::ResetPosition()
{
   State& state = GetCurrentState();

   Keyframe& keyframe = GetKeyframe(state, mSkeleton->time);
   if (mSkeleton->time != keyframe.time)
   {
      return;
   }

   keyframe.positions[mBone] = mSkeleton->bones[mBone].position;

   RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, mBone);
}

///
///
///
void Dock::ResetRotation()
{
   State& state = GetCurrentState();

   Keyframe& keyframe = GetKeyframe(state, mSkeleton->time);
   if (mSkeleton->time != keyframe.time)
   {
      return;
   }

   keyframe.rotations[mBone] = mSkeleton->bones[mBone].rotation;

   RecomputeBoneMatrix(state, keyframe, mSkeleton->bones, mBone);
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
   value = last;
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

   RecomputeBoneMatrix(state, keyframe, dock->mSkeleton->bones, bone);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

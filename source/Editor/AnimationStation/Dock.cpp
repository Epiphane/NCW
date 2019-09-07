// By Thomas Steinke

#include <algorithm>
#include <imgui.h>

#include <RGBText/StringHelper.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/UI/UIStackView.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/Button.h>
#include <Shared/UI/RectFilled.h>

#include "../Command/CommandStack.h"
#include "../Imgui/Extensions.h"

#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

using Bone = Skeleton::Bone;
using Keyframe = SkeletonAnimations::Keyframe;
using Stance = SimpleAnimationController::Stance;
using State = SimpleAnimationController::State;
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
static size_t GetKeyframeIndex(State& state, double time)
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
static Keyframe& GetKeyframe(State& state, double time)
{
   return state.keyframes[GetKeyframeIndex(state, time)];
}

}; // anonymous namespace

Dock::Dock(Engine::UIRoot* root, UIElement* parent)
    : UIElement(root, parent, "AnimationStationDock")
    , mBone(0)
    , mSelectedKeyframe(0)
{
   root->Subscribe<SuspendEditingEvent>(*this);
   root->Subscribe<ResumeEditingEvent>(*this);
   root->Subscribe<SkeletonLoadedEvent>(*this);
   root->Subscribe<SkeletonSelectedEvent>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<SimpleAnimationController>>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<AnimationSystemController>>(*this);
}

void Dock::Update(TIMEDELTA)
{
   if (!mController)
   {
      return;
   }

   // State selector
   ImGui::SetNextWindowPos(ImVec2(250, 550), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
   ImGui::Begin("State");

   ImVec2 space = ImGui::GetContentRegionAvail();
   ImGuiStyle& style = ImGui::GetStyle();
   const ImVec2 label_size = ImGui::CalcTextSize("X", NULL, true);
   float buttonSize = label_size.y + style.FramePadding.y * 2.0f;
   for (const auto& state : mStates)
   {
      if (ImGuiEx::Button(state == mController->current, state, ImVec2(space.x - buttonSize - style.ItemSpacing.x, buttonSize)))
      {
         SetState(state);
      }
      ImGui::SameLine();
      if (ImGuiEx::Button(false, "X", ImVec2(buttonSize, buttonSize)))
      {
         CommandStack::Instance().Do<RemoveStateCommand>(this);
      }
   }

   if (ImGui::Button("New State", ImVec2(space.x, buttonSize)))
   {
      CommandStack::Instance().Do<AddStateCommand>(this);
   }

   ImGui::End();

   ImGui::SetNextWindowPos(ImVec2(458, 550), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(200, 50), ImGuiCond_FirstUseEver);
   ImGui::Begin("State info");

   State& state = GetCurrentState();
   ImGui::Text("%.3f", state.length);
   // TODO edit

   ImGui::SameLine();
   if (ImGui::SmallButton("^"))
   {
      CommandStack::Instance().Do<SetStateLengthCommand>(this, state.length + 0.1);
   }
   ImGui::SameLine();
   if (state.length > 0.1 && ImGui::SmallButton("V"))
   {
      CommandStack::Instance().Do<SetStateLengthCommand>(this, state.length - 0.1);
   }

   ImGui::End();

   ImGui::SetNextWindowPos(ImVec2(664, 550), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(336, 0), ImGuiCond_FirstUseEver);
   ImGui::Begin("Timeline");

   ImGuiEx::SliderDouble("##time", &mController->time, 0.0, state.length);
   // TODO keyframes (custom drag behavior)

   size_t index = GetKeyframeIndex(state, mController->time);
   Keyframe& keyframe = state.keyframes[index];
   if (mController->time != keyframe.time && ImGui::Button("Add Keyframe"))
   {
      CommandStack::Instance().Do<AddKeyframeCommand>(this);
   }

   if (mController->time == keyframe.time && ImGuiEx::RedButton("Remove Keyframe"))
   {
      CommandStack::Instance().Do<RemoveKeyframeCommand>(this, index);
   }

   // TODO moving keyframes
   /*
      State& state = GetCurrentState();
      size_t index = mSelectedKeyframe;
      if (index >= state.keyframes.size())
      {
         return;
      }

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
      else if (keyframe.time > state.length)
      {
         keyframe.time = state.length;
      }

      mController->time = keyframe.time;
   */

   if (mSystemControls->paused && ImGui::Button("Play"))
   {
      mSystemControls->paused = false;
   }
   if (!mSystemControls->paused && ImGui::Button("Pause"))
   {
      mSystemControls->paused = true;
   }

   ImGui::Text("Speed: %.3f", mSystemControls->speed);
   ImGui::SameLine();
   if (ImGui::SmallButton("2x"))
   {
      mSystemControls->speed *= 2.0;
   }
   ImGui::SameLine();
   if (ImGui::SmallButton("/2"))
   {
      mSystemControls->speed /= 2.0;
   }

   ImGui::End();

   ImGui::SetNextWindowPos(ImVec2(664, 610), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(336, 150), ImGuiCond_FirstUseEver);
   ImGui::Begin("Bone");
   ImGui::Columns(2);
   float windowWidth = ImGui::GetWindowWidth();

   // Bone selector
   ImGui::SetColumnWidth(ImGui::GetColumnIndex(), windowWidth / 5);
   Stance& stance = GetCurrentStance();
   const Bone& selected = stance.bones[mBone];
   if (ImGui::BeginCombo("##bone", selected.name.c_str()))
   {
      for (size_t boneId = 0; boneId < stance.bones.size(); ++boneId)
      {
         const Bone& bone = stance.bones[boneId];
         bool isSelected = (mBone == boneId);
         if (ImGui::Selectable(bone.name.c_str(), isSelected))
         {
            SetBone(boneId);
         }
         if (isSelected)
         {
            ImGui::SetItemDefaultFocus();
         }
      }
      ImGui::EndCombo();
   }

   ImGui::NextColumn();
   ImGui::SetColumnWidth(ImGui::GetColumnIndex(), windowWidth * 4 / 5);

   if (mController->time == keyframe.time)
   {
      if (mScrubbers[0].Update("Position", keyframe.positions[selected.name], 0.1f))
      {
         OnScrub(ScrubType::Position, mScrubbers[0].GetLastValue());
      }
      ImGui::SameLine();
      if (ImGui::Button("Set base"))
      {
         CommandStack::Instance().Do<ResetBoneCommand>(this, mBone,
                                                       selected.position,
                                                       keyframe.rotations[selected.name],
                                                       keyframe.scales[selected.name]);
      }
      ImGui::SameLine();
      if (ImGui::Button("Set previous"))
      {
         Keyframe& prev = state.keyframes[index - 1];
         glm::vec3 pos = prev.positions.count(selected.name) != 0 ?
            prev.positions.at(selected.name) :
            selected.position;

         CommandStack::Instance().Do<ResetBoneCommand>(this, mBone,
                                                       pos,
                                                       keyframe.rotations[selected.name],
                                                       keyframe.scales[selected.name]);
      }

      if (mScrubbers[1].Update("Rotation", keyframe.rotations[selected.name]))
      {
         OnScrub(ScrubType::Rotation, mScrubbers[1].GetLastValue());
      }
      ImGui::SameLine();
      if (ImGui::Button("Set base"))
      {
         CommandStack::Instance().Do<ResetBoneCommand>(this, mBone,
                                                       keyframe.positions[selected.name],
                                                       selected.rotation,
                                                       keyframe.scales[selected.name]);
      }
      ImGui::SameLine();
      if (ImGui::Button("Set previous"))
      {
         Keyframe& prev = state.keyframes[index - 1];
         glm::vec3 rot = prev.rotations.count(selected.name) != 0 ?
            prev.rotations.at(selected.name) :
            selected.rotation;

         CommandStack::Instance().Do<ResetBoneCommand>(this, mBone,
                                                       keyframe.positions[selected.name],
                                                       rot,
                                                       keyframe.scales[selected.name]);
      }

      if (mScrubbers[2].Update("Scale", keyframe.scales[selected.name], 0.1f))
      {
         OnScrub(ScrubType::Scale, mScrubbers[2].GetLastValue());
      }
      ImGui::SameLine();
      if (ImGui::Button("Set base"))
      {
         CommandStack::Instance().Do<ResetBoneCommand>(this, mBone,
                                                       keyframe.positions[selected.name],
                                                       keyframe.rotations[selected.name],
                                                       selected.scale);
      }
      ImGui::SameLine();
      if (ImGui::Button("Set previous"))
      {
         Keyframe& prev = state.keyframes[index - 1];
         glm::vec3 scl = prev.scales.count(selected.name) != 0 ?
            prev.scales.at(selected.name) :
            selected.scale;

         CommandStack::Instance().Do<ResetBoneCommand>(this, mBone,
                                                       keyframe.positions[selected.name],
                                                       keyframe.rotations[selected.name],
                                                       scl);
      }
   }
   else
   {
      std::vector<std::string> parts = StringHelper::Split(selected.name, '.');
      const auto it = std::find_if(mController->skeletons.begin(), mController->skeletons.end(), [&](const auto& s) { return s->name == parts[0]; });
      assert(it != mController->skeletons.end() && "Couldn't find skeleton");
      Bone& bone = (*it)->bones[(*it)->boneLookup[selected.name]];

      mScrubbers[0].Update("Position", bone.position, 0.1f);
      mScrubbers[1].Update("Rotation", bone.rotation);
      mScrubbers[2].Update("Scale", bone.scale);
   }

   ImGui::End();
}

///
///
///
void Dock::Receive(const SuspendEditingEvent&)
{
   if (!mController)
   {
      return;
   }

   State& state = GetCurrentState();
   if (state.keyframes.empty() || mSelectedKeyframe >= state.keyframes.size() - 1)
   {
      return;
   }

   // Revert any pending edits so that they are not persisted.
   const Stance& stance = GetCurrentStance();
   const Bone& original = stance.bones[mBone];

   Keyframe& keyframe = state.keyframes[mSelectedKeyframe];
   if (keyframe.positions.count(original.name) > 0 && keyframe.positions.at(original.name) == original.position)
   {
      keyframe.positions.erase(original.name);
   }
   if (keyframe.rotations.count(original.name) > 0 && keyframe.rotations.at(original.name) == original.rotation)
   {
      keyframe.rotations.erase(original.name);
   }
   if (keyframe.scales.count(original.name) > 0 && keyframe.scales.at(original.name) == original.scale)
   {
      keyframe.scales.erase(original.name);
   }
}

///
///
///
void Dock::Receive(const ResumeEditingEvent&)
{
   State& state = GetCurrentState();
   if (mSelectedKeyframe >= state.keyframes.size())
   {
      return;
   }

   Keyframe& keyframe = state.keyframes[mSelectedKeyframe];

   const Stance& stance = GetCurrentStance();
   const Bone& original = stance.bones[mBone];
   if (keyframe.positions.count(original.name) == 0)
   {
      keyframe.positions.emplace(original.name, original.position);
   }
   if (keyframe.rotations.count(original.name) == 0)
   {
      keyframe.rotations.emplace(original.name, original.rotation);
   }
   if (keyframe.scales.count(original.name) == 0)
   {
      keyframe.scales.emplace(original.name, original.scale);
   }
}

///
///
///
void Dock::Receive(const SkeletonLoadedEvent& evt)
{
   mController = evt.component;

   mStates.clear();
   for (const auto&[name, _] : mController->states)
   {
      mStates.push_back(name);
   }
   std::sort(mStates.begin(), mStates.end());

   SetState(mStates[0]);
   SetTime(0);
   SetBone(0);
}

///
///
///
void Dock::Receive(const SkeletonSelectedEvent& evt)
{
   mSkeleton = evt.component->name;
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<SimpleAnimationController>& evt)
{
   mController = evt.component;
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
void Dock::OnScrub(ScrubType type, glm::vec3 oldValue)
{
   // Funky time: at this point, the current value represents the NEW state,
   // and we create a command to set it to the OLD state. So we perform the
   // command twice, once immediately to revert to the old state, and then
   // again when it gets placed on the stack to go back to the new one,

   const Bone& selected = GetCurrentStance().bones[mBone];
   Keyframe& keyframe = GetCurrentKeyframe();
   glm::vec3 pos = keyframe.positions[selected.name];
   glm::vec3 rot = keyframe.rotations[selected.name];
   glm::vec3 scl = keyframe.scales[selected.name];

   switch (type)
   {
   case ScrubType::Position:
      pos = oldValue;
      break;
   case ScrubType::Rotation:
      rot = oldValue;
      break;
   case ScrubType::Scale:
      scl = oldValue;
      break;
   }

   std::unique_ptr<ResetBoneCommand> command{new ResetBoneCommand(this, mBone, pos, rot, scl)};
   command->Do();
   CommandStack::Instance().Do(std::move(command));
}

///
///
///
State& Dock::GetCurrentState()
{
   return mController->states[mController->current];
}

///
///
///
Stance& Dock::GetCurrentStance()
{
   return mController->stances[GetCurrentState().stance];
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
void Dock::SetState(const std::string& name)
{
   if (mController->current == name)
   {
      return;
   }

   Receive(SuspendEditingEvent{});

   mController->current = name;
   SetTime(mController->time);

   Receive(ResumeEditingEvent{});
}

///
///
///
void Dock::SetBone(const size_t& boneId)
{
   if (mBone == boneId)
   {
      return;
   }

   Receive(SuspendEditingEvent{});

   mBone = boneId;

   Receive(ResumeEditingEvent{});
}

///
///
///
void Dock::AddStateCommand::Do()
{
   if (state.entity.empty())
   {
      state.entity = dock->mSkeleton;
   }

   if (state.stance.empty())
   {
      state.stance = "base";
   }

   if (state.keyframes.size() == 0)
   {
      Keyframe keyframe;
      keyframe.time = 0;
      state.keyframes.push_back(keyframe);
   }

   dock->mController->states.emplace(state.name, state);
   dock->mStates.push_back(state.name);
   std::sort(dock->mStates.begin(), dock->mStates.end());
   dock->SetState(state.name);
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::AddStateCommand::Undo()
{
   // Get current state as a copy not a reference
   state = dock->GetCurrentState();

   dock->mController->states.erase(state.name);
   size_t index = size_t(std::find(dock->mStates.begin(), dock->mStates.end(), state.name) - dock->mStates.begin());
   if (index > 0)
   {
      --index;
   }
   dock->mStates.erase(dock->mStates.begin() + (int64_t)index);
   dock->SetState(dock->mStates[index]);
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::SetStateLength(double newValue, double oldValue)
{
   State& state = GetCurrentState();
   state.length = newValue;

   double stretch = newValue / oldValue;

   for (Keyframe& keyframe : state.keyframes)
   {
      keyframe.time *= stretch;
   }

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

   dock->mController->states.emplace(state.name, dock->mController->states.at(last));
   for (std::string& entry : dock->mStates)
   {
      if (entry == last)
      {
         entry = name;
         break;
      }
   }
   dock->SetState(name);
   dock->mController->states.erase(last);

   name = last;

   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

//
///
///
void Dock::SetStateLengthCommand::Do()
{
   State& state = dock->GetCurrentState();
   double prev = state.length;
   dock->SetStateLength(value, prev);
   value = prev;
}

///
///
///
void Dock::AddKeyframeCommand::Do()
{
   State& state = dock->GetCurrentState();
   Stance& stance = dock->mController->stances[state.stance];

   // If this keyframe did not already exist, populate it with the current values.
   if (keyframe.positions.empty() && keyframe.rotations.empty() && keyframe.scales.empty())
   {
      // Insert the CURRENT transform values at this timestamp.
      size_t boneId = 0;
      for (const Engine::ComponentHandle<Skeleton>& skeleton : dock->mController->skeletons)
      {
         for (const Skeleton::Bone& bone : skeleton->bones)
         {
            if (bone.position != stance.bones[boneId].position)
            {
               keyframe.positions[bone.name] = bone.position;
            }
            if (bone.rotation != stance.bones[boneId].rotation)
            {
               keyframe.rotations[bone.name] = bone.rotation;
            }
            if (bone.scale != stance.bones[boneId].scale)
            {
               keyframe.scales[bone.name] = bone.scale;
            }
            ++boneId;
         }
      }
   }

   keyframeIndex = GetKeyframeIndex(state, dock->mController->time) + 1;
   state.keyframes.insert(state.keyframes.begin() + (int64_t)keyframeIndex, keyframe);
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
   state.keyframes.erase(state.keyframes.begin() + (int64_t)keyframeIndex);
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::SetTime(double time)
{
   State& state = GetCurrentState();

   Receive(SuspendEditingEvent{});

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
       (state.keyframes[keyframeIndex + 1].time - time) / state.length < 0.02)
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

   Receive(ResumeEditingEvent{});
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
   value = last;

   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::ResetBoneCommand::Do()
{
   Keyframe& keyframe = dock->GetCurrentKeyframe();
   if (dock->mController->time != keyframe.time)
   {
      return;
   }

   const Stance& stance = dock->GetCurrentStance();
   const Bone& bone = stance.bones[dock->mBone];

   assert(keyframe.positions.count(bone.name) > 0 && "Not editing a bone");
   assert(keyframe.rotations.count(bone.name) > 0 && "Not editing a bone");
   assert(keyframe.scales.count(bone.name) > 0 && "Not editing a bone");

   glm::vec3 pos = keyframe.positions[bone.name];
   glm::vec3 rot = keyframe.rotations[bone.name];
   glm::vec3 scl = keyframe.scales[bone.name];
   keyframe.positions[bone.name] = position;
   keyframe.rotations[bone.name] = rotation;
   keyframe.scales[bone.name] = scale;
   position = pos;
   rotation = rot;
   scale = scl;

   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

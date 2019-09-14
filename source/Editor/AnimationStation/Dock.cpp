// By Thomas Steinke

#include <algorithm>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <RGBDesignPatterns/CommandStack.h>
#include <RGBText/StringHelper.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/UI/UIStackView.h>
#include <Shared/Helpers/Asset.h>

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
   ImGui::SetNextWindowPos(ImVec2(25, 140), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
   ImGui::Begin("State");

   ImVec2 space = ImGui::GetContentRegionAvail();
   ImGuiStyle& style = ImGui::GetStyle();
   const ImVec2 label_size = ImGui::CalcTextSize("X", NULL, true);
   float buttonSize = label_size.y + style.FramePadding.y * 2.0f;
   for (const auto&[name, _] : mController->states)
   {
      if (ImGuiEx::Button(name == mController->current, name, ImVec2(space.x - buttonSize - style.ItemSpacing.x, buttonSize)))
      {
         SetState(name);
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

   ImGui::SetNextWindowPos(ImVec2(975, 20), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(275, 100), ImGuiCond_FirstUseEver);
   ImGui::Begin("State info");

   {
      State& state = GetCurrentState();
      if (mStateName.Update("Name", state.name))
      {
         mController->states.emplace(state.name, state);
         CommandStack::Instance().Emplace<SetStateNameCommand>(this, state.name, mStateName.GetLastValue());
         SetState(state.name);
         mController->states.erase(mStateName.GetLastValue());
      }
   }

   State& state = GetCurrentState();
   ImGui::Text("%.3f", state.length);

   ImGui::SameLine();
   if (ImGui::SmallButton("^"))
   {
      CommandStack::Instance().Do<SetStateLengthCommand>(this, state.name, state.length + 0.1);
   }
   ImGui::SameLine();
   if (state.length > 0.1 && ImGui::SmallButton("V"))
   {
      CommandStack::Instance().Do<SetStateLengthCommand>(this, state.name, state.length - 0.1);
   }

   if (ImGui::BeginCombo("##stance", state.stance.c_str()))
   {
      for (const auto& [stance, _] : mController->stances)
      {
         bool isSelected = (state.stance == stance);
         if (ImGui::Selectable(stance.c_str(), isSelected))
         {
            CommandStack::Instance().Do<SetStanceCommand>(this, state.name, stance);
         }
         if (isSelected)
         {
            ImGui::SetItemDefaultFocus();
         }
      }
      ImGui::EndCombo();
   }

   ImGui::End();

   ImGui::SetNextWindowPos(ImVec2(975, 136), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(275, 200), ImGuiCond_FirstUseEver);
   ImGui::Begin("Timeline");

   std::vector<double> keyframes;
   for (const Keyframe& frame : state.keyframes)
   {
      keyframes.push_back(frame.time);
   }

   Receive(SuspendEditingEvent{});
   ImGuiEx::Timeline("##time", &mController->time, state.length, mSystemControls->paused, keyframes);

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
   ImGui::Checkbox("Seamless loop", &mSystemControls->seamlessLoop);

   ImGui::End();

   // Keyframe modification
   ImGui::SetNextWindowPos(ImVec2(975, 350), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(275, 80), ImGuiCond_FirstUseEver);
   ImGui::Begin("Keyframe");

   size_t index = GetKeyframeIndex(state, mController->time);
   Keyframe& keyframe = state.keyframes[index];
   if (mController->time != keyframe.time)
   {
      if (ImGui::Button("Add Keyframe"))
      {
         CommandStack::Instance().Do<AddKeyframeCommand>(this, state.name);
      }
   }
   else
   {
      if (ImGui::Button("Remove Keyframe"))
      {
         CommandStack::Instance().Do<RemoveKeyframeCommand>(this, state.name, index);
      }

      double min = index > 0 ? state.keyframes[index - 1].time + 0.01 : 0.0;
      double max = index < state.keyframes.size() - 1 ? state.keyframes[index + 1].time - 0.01 : state.length;

      if (mKeyframeTimeScrubber.Slide("Time", keyframe.time, min, max))
      {
         CommandStack::Instance().Emplace<SetKeyframeTimeCommand>(
            this,
            state.name,
            index,
            mKeyframeTimeScrubber.GetLastValue()
         );
      }

      if (ImGui::IsItemActive())
      {
         mController->time = keyframe.time;
      }
   }

   ImGui::End();

   ImGui::SetNextWindowPos(ImVec2(250, 550), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(1000, 200), ImGuiCond_FirstUseEver);
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
      Receive(ResumeEditingEvent{});

      if (mScrubbers[0].Drag("Position", keyframe.positions[selected.name], 0.1f))
      {
         OnScrub(ScrubType::Position, mScrubbers[0].GetLastValue());
         Receive(ResumeEditingEvent{});
      }
      ImGui::SameLine();
      if (ImGui::Button("Set base"))
      {
         CommandStack::Instance().Do<ResetBoneCommand>(this, state.name, index, mBone,
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

         CommandStack::Instance().Do<ResetBoneCommand>(this, state.name, index, mBone,
                                                       pos,
                                                       keyframe.rotations[selected.name],
                                                       keyframe.scales[selected.name]);
      }

      if (mScrubbers[1].Drag("Rotation", keyframe.rotations[selected.name]))
      {
         OnScrub(ScrubType::Rotation, mScrubbers[1].GetLastValue());
         Receive(ResumeEditingEvent{});
      }
      ImGui::SameLine();
      if (ImGui::Button("Set base"))
      {
         CommandStack::Instance().Do<ResetBoneCommand>(this, state.name, index, mBone,
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

         CommandStack::Instance().Do<ResetBoneCommand>(this, state.name, index, mBone,
                                                       keyframe.positions[selected.name],
                                                       rot,
                                                       keyframe.scales[selected.name]);
      }

      if (mScrubbers[2].Drag("Scale", keyframe.scales[selected.name], 0.1f))
      {
         OnScrub(ScrubType::Scale, mScrubbers[2].GetLastValue());
         Receive(ResumeEditingEvent{});
      }
      ImGui::SameLine();
      if (ImGui::Button("Set base"))
      {
         CommandStack::Instance().Do<ResetBoneCommand>(this, state.name, index, mBone,
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

         CommandStack::Instance().Do<ResetBoneCommand>(this, state.name, index, mBone,
                                                       keyframe.positions[selected.name],
                                                       keyframe.rotations[selected.name],
                                                       scl);
      }

      // Revert any unchanged vectors so that they are not persisted.
      Receive(SuspendEditingEvent{});
   }
   else
   {
      std::vector<std::string> parts = StringHelper::Split(selected.name, '.');
      const auto it = std::find_if(mController->skeletons.begin(), mController->skeletons.end(), [&](const auto& s) { return s->name == parts[0]; });
      assert(it != mController->skeletons.end() && "Couldn't find skeleton");
      Bone& bone = (*it)->bones[(*it)->boneLookup[selected.name]];

      mScrubbers[0].Drag("Position", bone.position, 0.1f);
      mScrubbers[1].Drag("Rotation", bone.rotation);
      mScrubbers[2].Drag("Scale", bone.scale);
   }

   ImGui::End();
}

///
///
///
void Dock::Receive(const SuspendEditingEvent&)
{
   if (!mController || mController->current.empty() || GetCurrentState().keyframes.empty())
   {
      return;
   }

   Keyframe& keyframe = GetCurrentKeyframe();
   if (keyframe.time != mController->time)
   {
      return;
   }

   // Revert any pending edits so that they are not persisted.
   const Stance& stance = GetCurrentStance();
   const Bone& original = stance.bones[mBone];

   if (keyframe.positions.count(original.name) > 0 &&
      glm::all(glm::equal(keyframe.positions.at(original.name), original.position)))
   {
      keyframe.positions.erase(original.name);
   }
   if (keyframe.rotations.count(original.name) > 0 &&
      glm::all(glm::equal(keyframe.rotations.at(original.name), original.rotation)))
   {
      keyframe.rotations.erase(original.name);
   }
   if (keyframe.scales.count(original.name) > 0 &&
      glm::all(glm::equal(keyframe.scales.at(original.name), original.scale)))
   {
      keyframe.scales.erase(original.name);
   }
}

///
///
///
void Dock::Receive(const ResumeEditingEvent&)
{
   if (!mController || mController->current.empty() || GetCurrentState().keyframes.empty())
   {
      return;
   }

   Keyframe& keyframe = GetCurrentKeyframe();
   if (keyframe.time != mController->time)
   {
      return;
   }

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

   SetState(mController->states.begin()->first);
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
   State& state = GetCurrentState();
   size_t keyframeIndex = GetKeyframeIndex(state, mController->time);
   Keyframe& keyframe = state.keyframes[keyframeIndex];
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

   CommandStack::Instance().Emplace<ResetBoneCommand>(this, state.name, keyframeIndex, mBone, pos, rot, scl);
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
   if (state.name.empty())
   {
      state.name = Format::FormatString("Unnamed state %1", dock->mController->states.size());
   }

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
   dock->SetState(dock->mController->states.begin()->first);
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

   mController->time *= stretch;
   mpRoot->Emit<SkeletonModifiedEvent>(mController);
}

///
///
///
void Dock::SetStateNameCommand::Do()
{
   State state = dock->mController->states[prev];

   std::string last = state.name;
   state.name = name;

   dock->mController->states.emplace(name, state);
   dock->SetState(name);
   dock->mController->states.erase(prev);

   prev = name;
   name = last;

   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

//
///
///
void Dock::SetStateLengthCommand::Do()
{
   dock->SetState(stateName);
   State& state = dock->GetCurrentState();
   double prev = state.length;
   dock->SetStateLength(value, prev);
   value = prev;
}

//
///
///
void Dock::SetStanceCommand::Do()
{
   dock->SetState(stateName);
   State& state = dock->GetCurrentState();
   std::string prev = state.stance;
   state.stance = stance;
   stance = prev;
}

///
///
///
Dock::AddKeyframeCommand::AddKeyframeCommand(Dock* dock, const std::string& state)
   : DockCommand(dock)
   , stateName(state)
   , keyframeIndex(0)
{
   keyframe.time = dock->mController->time;
   keyframeIndex = GetKeyframeIndex(dock->GetCurrentState(), dock->mController->time) + 1;
};

void Dock::AddKeyframeCommand::Do()
{
   dock->SetState(stateName);
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

   state.keyframes.insert(state.keyframes.begin() + (int64_t)keyframeIndex, keyframe);
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

void Dock::AddKeyframeCommand::Undo()
{
   dock->SetState(stateName);
   State& state = dock->GetCurrentState();

   // Copy by value not reference
   keyframe = state.keyframes[keyframeIndex];
   state.keyframes.erase(state.keyframes.begin() + (int64_t)keyframeIndex);
   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

///
///
///
void Dock::SetKeyframeTimeCommand::Do()
{
   dock->SetState(stateName);
   State& state = dock->GetCurrentState();
   Keyframe& keyframe = state.keyframes[index];

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
   dock->SetState(stateName);
   Keyframe& keyframe = dock->GetCurrentState().keyframes[keyframeIndex];
   dock->mController->time = keyframe.time;

   const Stance& stance = dock->GetCurrentStance();
   const Bone& bone = stance.bones[dock->mBone];

   dock->Receive(ResumeEditingEvent{});
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

   dock->Receive(SuspendEditingEvent{});

   dock->mpRoot->Emit<SkeletonModifiedEvent>(dock->mController);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

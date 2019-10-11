// By Thomas Steinke

#include <algorithm>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <RGBDesignPatterns/CommandStack.h>
#include <RGBText/StringHelper.h>
#include <Engine/UI/UIStackView.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/Button.h>
#include <Shared/UI/RectFilled.h>

#include "../Imgui/Extensions.h"
#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

using Bone = Skeleton::Bone;
using Engine::UIElement;
using Engine::UIFrame;
using Engine::UIStackView;

Dock::Dock(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent, "SkeletorDock")
   , mBone("none.root")
{
   root->Subscribe<SuspendEditingEvent>(*this);
   root->Subscribe<ResumeEditingEvent>(*this);
   root->Subscribe<SkeletonClearedEvent>(*this);
   root->Subscribe<SkeletonLoadedEvent>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<SkeletonCollection>>(*this);
}

void Dock::Update(TIMEDELTA)
{
   if (!mSkeleton || !mSkeletons)
   {
      return;
   }

   // Select defaults
   if (std::find_if(mSkeleton->stances.begin(), mSkeleton->stances.end(), [&](const auto& s) { return s.name == mStance; }) == mSkeleton->stances.end())
   {
      SetStance(mSkeleton->stances[0].name);
   }

   if (std::find_if(mSkeleton->bones.begin(), mSkeleton->bones.end(), [&](const auto& s) { return s.name == mBone; }) == mSkeleton->bones.end())
   {
      SetBone(mSkeleton->bones[0].name);
   }

   // Stance information
   ImGui::SetNextWindowPos(ImVec2(250, 550), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(1000, 200), ImGuiCond_FirstUseEver);
   ImGui::Begin("Stance");
   ImGui::Columns(3);
   float windowWidth = ImGui::GetWindowWidth();

   // Stance selector
   ImGui::SetColumnWidth(ImGui::GetColumnIndex(), windowWidth / 5);
   ImVec2 space = ImGui::GetContentRegionAvail();
   for (const auto& stance : mSkeleton->stances)
   {
      if (ImGuiEx::Button(mStance == stance.name, stance.name, ImVec2(space.x, 0)))
      {
         SetStance(stance.name);
      }
   }

   // Bone selector
   ImGui::NextColumn();
   ImGui::SetColumnWidth(ImGui::GetColumnIndex(), windowWidth / 5);
   space = ImGui::GetContentRegionAvail();
   for (const auto& bone : mSkeleton->bones)
   {
      if (ImGuiEx::Button(mBone == bone.name, bone.name, ImVec2(space.x, 0)))
      {
         SetBone(bone.name);
      }
   }

   // Position, rotation, scale
   ImGui::NextColumn();
   ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 3 * windowWidth / 5);

   auto stance = std::find_if(mSkeleton->stances.begin(), mSkeleton->stances.end(), [&](const auto& s) { return s.name == mStance; });
   if (mScrubbers[0].Drag("Position", stance->positions[mBone], 0.1f))
   {
      OnScrub(ScrubType::Position, mScrubbers[0].GetLastValue());
   }
   if (mScrubbers[1].Drag("Rotation", stance->rotations[mBone]))
   {
      OnScrub(ScrubType::Position, mScrubbers[0].GetLastValue());
   }
   if (mScrubbers[2].Drag("Scale", stance->scales[mBone], 0.1f))
   {
      OnScrub(ScrubType::Position, mScrubbers[0].GetLastValue());
   }

   ImGui::End();
}

///
///
///
void Dock::SetStance(const std::string& stance)
{
   Receive(SuspendEditingEvent{});

   mStance = stance;

   if (mSkeletons)
   {
      mSkeletons->stance = mStance;
   }

   Receive(ResumeEditingEvent{});
}

void Dock::SetBone(const std::string& bone)
{
   Receive(SuspendEditingEvent{});

   mBone = bone;

   Receive(ResumeEditingEvent{});
}

///
///
///
void Dock::Receive(const SkeletonClearedEvent&)
{
}

///
///
///
void Dock::Receive(const SkeletonLoadedEvent& evt)
{
   mSkeleton = evt.component;

   // Clear all bindings
   Receive(SkeletonClearedEvent{});

   SetStance(mStance);
}

void Dock::Receive(const Engine::ComponentAddedEvent<SkeletonCollection>& evt)
{
   mSkeletons = evt.component;

   SetStance(mStance);
}

void Dock::Receive(const SuspendEditingEvent&)
{
   if (!mSkeleton || !mSkeletons)
   {
      return;
   }

   if (mSkeleton->boneLookup.count(mBone) == 0)
   {
      return;
   }

   // Revert any pending edits so that they are not persisted.
   Skeleton::Stance& stance = mSkeleton->stances[mSkeleton->stanceLookup[mStance]];
   bool changed[4] = {false}; // position, rotation, scale, parent
   bool checked[4] = {false}; // position, rotation, scale, parent
   for (std::string it = mSkeletons->parents[stance.name]; !it.empty(); it = mSkeletons->parents[it])
   {
      Skeleton::Stance& parent = mSkeleton->stances[mSkeleton->stanceLookup[it]];
      if (!checked[0] && parent.positions.count(mBone) != 0)
      {
         changed[0] = parent.positions[mBone] != stance.positions[mBone];
         checked[0] = true;
      }
      if (!checked[1] && parent.rotations.count(mBone) != 0)
      {
         changed[1] = parent.rotations[mBone] != stance.rotations[mBone];
         checked[1] = true;
      }
      if (!checked[2] && parent.scales.count(mBone) != 0)
      {
         changed[2] = parent.scales[mBone] != stance.scales[mBone];
         checked[2] = true;
      }
      if (!checked[3] && parent.parents.count(mBone) != 0)
      {
         changed[3] = parent.parents[mBone] != stance.parents[mBone];
         checked[3] = true;
      }
   }

   const Skeleton::Bone& original = mSkeleton->original[mSkeleton->boneLookup[mBone]];

   if (!checked[0])
   {
      changed[0] = !glm::all(glm::equal(original.position, stance.positions[original.name], FLT_EPSILON));
      checked[0] = true;
   }
   if (!checked[1])
   {
      changed[1] = !glm::all(glm::equal(original.rotation, stance.rotations[original.name], FLT_EPSILON));
      checked[1] = true;
   }
   if (!checked[2])
   {
      changed[2] = !glm::all(glm::equal(original.scale, stance.scales[original.name], FLT_EPSILON));
      checked[2] = true;
   }
   if (!checked[3])
   {
      changed[3] = original.parent != stance.parents[original.name];
      checked[3] = true;
   }

   if (!changed[0])
   {
      stance.positions.erase(mBone);
   }
   if (!changed[1])
   {
      stance.rotations.erase(mBone);
   }
   if (!changed[2])
   {
      stance.scales.erase(mBone);
   }
   if (!changed[3])
   {
      stance.parents.erase(mBone);
   }
}

void Dock::Receive(const ResumeEditingEvent&)
{
   if (!mSkeleton || !mSkeletons)
   {
      return;
   }

   if (mSkeleton->boneLookup.count(mBone) == 0)
   {
      return;
   }

   Skeleton::Stance& stance = mSkeleton->stances[mSkeleton->stanceLookup[mStance]];
   bool set[4] = {false}; // position, rotation, scale, parent
   for (std::string it = stance.name; !it.empty(); it = mSkeletons->parents[it])
   {
      Skeleton::Stance& s = mSkeleton->stances[mSkeleton->stanceLookup[it]];
      if (!set[0] && s.positions.count(mBone) != 0)
      {
         stance.positions[mBone] = s.positions[mBone];
         set[0] = true;
      }
      if (!set[1] && s.rotations.count(mBone) != 0)
      {
         stance.rotations[mBone] = s.rotations[mBone];
         set[1] = true;
      }
      if (!set[2] && s.scales.count(mBone) != 0)
      {
         stance.scales[mBone] = s.scales[mBone];
         set[2] = true;
      }
      if (!set[3] && s.parents.count(mBone) != 0)
      {
         stance.parents[mBone] = s.parents[mBone];
         set[3] = true;
      }
   }

   const Skeleton::Bone& original = mSkeleton->original[mSkeleton->boneLookup[mBone]];
   if (!set[0])
   {
      stance.positions[mBone] = original.position;
      set[0] = true;
   }
   if (!set[1])
   {
      stance.rotations[mBone] = original.rotation;
      set[1] = true;
   }
   if (!set[2])
   {
      stance.scales[mBone] = original.scale;
      set[2] = true;
   }
   if (!set[3])
   {
      stance.parents[mBone] = original.parent;
      set[3] = true;
   }
}

///
///
///
void Dock::OnScrub(ScrubType type, glm::vec3 oldValue)
{
   // Funky time: at this point, the current value represents the NEW state,
   // and we create a command to set it to the OLD state.
   CommandStack::Instance().Emplace<SetBoneCommand>(this, mStance, mBone, type, oldValue);
}

///
///
///
void Dock::SetBoneCommand::Do()
{
   dock->SetStance(stance);
   dock->SetBone(bone);

   auto currentStance = std::find_if(dock->mSkeleton->stances.begin(), dock->mSkeleton->stances.end(), [&](const auto& s) { return s.name == dock->mStance; });
   glm::vec3* vec = nullptr;
   switch (type)
   {
   case ScrubType::Position:
      vec = &currentStance->positions[dock->mBone];
      break;
   case ScrubType::Rotation:
      vec = &currentStance->rotations[dock->mBone];
      break;
   case ScrubType::Scale:
      vec = &currentStance->scales[dock->mBone];
      break;
   }

   assert(vec != nullptr && "Invalid type");

   glm::vec3 prev = value;
   value = *vec;
   *vec = prev;
}

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

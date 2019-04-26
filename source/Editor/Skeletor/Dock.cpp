// By Thomas Steinke

#include <algorithm>
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

namespace Skeletor
{

using Bone = Skeleton::Bone;
using Engine::UIElement;
using Engine::UIFrame;
using Engine::UIStackView;
using UI::Button;
using UI::RectFilled;

Dock::Dock(Engine::UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "SkeletorDock", glm::vec4(0.2, 0.2, 0.2, 1))
   , mBone("")
{
   RectFilled* foreground = Add<RectFilled>("SkeletorDockFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainCenterTo(this);
   foreground->ConstrainDimensionsTo(this, -4);

   // Bone information
   UIElement* boneHeader = Add<UIStackView>();
   boneHeader->ConstrainLeftAlignedTo(foreground, 30);
   boneHeader->ConstrainTopAlignedTo(foreground, 30);
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
   }

   // Bone positions, rotations and sliders
   UIElement* bonePosition = Add<UIElement>();
   UIElement* boneRotation = Add<UIElement>();
   UIElement* boneScale = Add<UIElement>();
   bonePosition->ConstrainBelow(boneHeader, 8);
   bonePosition->ConstrainLeftAlignedTo(boneHeader);

   boneRotation->ConstrainTopAlignedTo(bonePosition);
   boneRotation->ConstrainToRightOf(bonePosition, 16);
   boneRotation->ConstrainWidthTo(bonePosition);

   boneScale->ConstrainTopAlignedTo(boneRotation);
   boneScale->ConstrainRightAlignedTo(boneHeader);
   boneScale->ConstrainToRightOf(boneRotation, 16);
   boneScale->ConstrainWidthTo(boneRotation);

   {
      // Position/rotation icons that look good
      Image* position = bonePosition->Add<Image>(Image::Options{Asset::Image("EditorIcons.png"), "position"});
      Image* rotation = boneRotation->Add<Image>(Image::Options{Asset::Image("EditorIcons.png"), "rotation"});
      Image* scale = boneScale->Add<Image>(Image::Options{Asset::Image("EditorIcons.png"), "scale"});

      position->ConstrainWidth(37);
      position->ConstrainLeftAlignedTo(bonePosition);
      position->ConstrainVerticalCenterTo(bonePosition);
      rotation->ConstrainWidth(40);
      rotation->ConstrainLeftAlignedTo(boneRotation);
      rotation->ConstrainVerticalCenterTo(boneRotation);
      scale->ConstrainWidth(37);
      scale->ConstrainLeftAlignedTo(boneScale);
      scale->ConstrainVerticalCenterTo(boneScale);

      // Bone position/rotation reset buttons
      Button::Options buttonOptions;
      buttonOptions.filename = Asset::Image("EditorIcons.png");
      buttonOptions.image = "reset";
      buttonOptions.onClick = [&]() {
         // TODO
      };
      Button* resetPositionButton = bonePosition->Add<Button>(buttonOptions);
      resetPositionButton->ConstrainWidth(35);
      resetPositionButton->ConstrainLeftAlignedTo(position);
      resetPositionButton->ConstrainBelow(position, 8);

      buttonOptions.onClick = [&]() {
         // TODO
      };
      Button* resetRotationButton = boneRotation->Add<Button>(buttonOptions);
      resetRotationButton->ConstrainWidth(35);
      resetRotationButton->ConstrainLeftAlignedTo(rotation);
      resetRotationButton->ConstrainBelow(rotation, 8);

      // Bone position/rotation controls
      NumDisplay<float>::Options textOptions;
      textOptions.text = "0.0";
      textOptions.precision = 1;

      Scrubber<float>::Options scrubberOptions;
      scrubberOptions.filename = Asset::Image("EditorIcons.png");
      scrubberOptions.image = "drag_number";
      scrubberOptions.onChange = [&](double, double) {
         /*
         Bone& bone = mSkeleton->bones[mBone];

         bone.originalPosition = bone.position;
         bone.originalRotation = bone.rotation;
         bone.originalScale = bone.scale;
         */

         mpRoot->Emit<SkeletonModifiedEvent>(mSkeleton);
      };
      scrubberOptions.sensitivity = 0.02;

      UIStackView* positionScrubbers = bonePosition->Add<UIStackView>("PositionScrubbers");
      positionScrubbers->ConstrainToRightOf(resetPositionButton, 16);
      positionScrubbers->ConstrainHeightTo(bonePosition);
      positionScrubbers->ConstrainTopAlignedTo(bonePosition);
      positionScrubbers->ConstrainRightAlignedTo(bonePosition, 12);
      UIStackView* rotationScrubbers = boneRotation->Add<UIStackView>("RotationScrubbers");
      rotationScrubbers->ConstrainToRightOf(resetRotationButton, 16);
      rotationScrubbers->ConstrainHeightTo(boneRotation);
      rotationScrubbers->ConstrainTopAlignedTo(boneRotation);
      rotationScrubbers->ConstrainRightAlignedTo(boneRotation, 12);
      UIStackView* scaleScrubbers = boneScale->Add<UIStackView>("ScaleScrubbers");
      scaleScrubbers->ConstrainToRightOf(scale, 16);
      scaleScrubbers->ConstrainHeightTo(boneScale);
      scaleScrubbers->ConstrainTopAlignedTo(boneScale);
      scaleScrubbers->ConstrainRightAlignedTo(boneScale, 12);

      for (int i = 0; i < 3; i++)
      {
         mBonePos[i].text = positionScrubbers->Add<NumDisplay<float>>(textOptions);
         mBonePos[i].scrubber = positionScrubbers->Add<Scrubber<float>>(scrubberOptions);
         mBoneRot[i].text = rotationScrubbers->Add<NumDisplay<float>>(textOptions);
         mBoneRot[i].scrubber = rotationScrubbers->Add<Scrubber<float>>(scrubberOptions);
         mBoneScl[i].text = scaleScrubbers->Add<NumDisplay<float>>(textOptions);
         mBoneScl[i].scrubber = scaleScrubbers->Add<Scrubber<float>>(scrubberOptions);

         mBonePos[i].text->ConstrainHeight(32);
         mBonePos[i].text->ConstrainLeftAlignedTo(positionScrubbers);
         mBonePos[i].scrubber->ConstrainHeight(7);
         mBonePos[i].scrubber->ConstrainLeftAlignedTo(positionScrubbers);
         mBoneRot[i].text->ConstrainHeight(32);
         mBoneRot[i].text->ConstrainLeftAlignedTo(rotationScrubbers);
         mBoneRot[i].scrubber->ConstrainHeight(7);
         mBoneRot[i].scrubber->ConstrainLeftAlignedTo(rotationScrubbers);
         mBoneScl[i].text->ConstrainHeight(32);
         mBoneScl[i].text->ConstrainLeftAlignedTo(scaleScrubbers);
         mBoneScl[i].scrubber->ConstrainHeight(7);
         mBoneScl[i].scrubber->ConstrainLeftAlignedTo(scaleScrubbers);
      }

      mBonePos[0].scrubber->ConstrainRightAlignedTo(bonePosition);
      mBoneRot[0].scrubber->ConstrainRightAlignedTo(boneRotation);
      mBoneScl[0].scrubber->ConstrainRightAlignedTo(boneScale);
   }

   root->Subscribe<SuspendEditingEvent>(*this);
   root->Subscribe<ResumeEditingEvent>(*this);
   root->Subscribe<SkeletonClearedEvent>(*this);
   root->Subscribe<SkeletonLoadedEvent>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<SkeletonCollection>>(*this);

   SetBone("root");
   SetStance("resting");
}

///
///
///
void Dock::Receive(const SkeletonClearedEvent&)
{
   mBonePos[0].text->Bind(nullptr);
   mBonePos[1].text->Bind(nullptr);
   mBonePos[2].text->Bind(nullptr);
   mBoneRot[0].text->Bind(nullptr);
   mBoneRot[1].text->Bind(nullptr);
   mBoneRot[2].text->Bind(nullptr);
   mBoneScl[0].text->Bind(nullptr);
   mBoneScl[1].text->Bind(nullptr);
   mBoneScl[2].text->Bind(nullptr);

   mBonePos[0].scrubber->Bind(nullptr);
   mBonePos[1].scrubber->Bind(nullptr);
   mBonePos[2].scrubber->Bind(nullptr);
   mBoneRot[0].scrubber->Bind(nullptr);
   mBoneRot[1].scrubber->Bind(nullptr);
   mBoneRot[2].scrubber->Bind(nullptr);
   mBoneScl[0].scrubber->Bind(nullptr);
   mBoneScl[1].scrubber->Bind(nullptr);
   mBoneScl[2].scrubber->Bind(nullptr);
}

///
///
///
void Dock::Receive(const SkeletonLoadedEvent& evt)
{
   mSkeleton = evt.component;

   mStances.clear();
   if (mSkeletons)
   {
      // Every stance has a parent
      for (const auto& entry : mSkeletons->parents)
      {
         mStances.push_back(entry.first);
      }
   }
   mBone = "root";
   Receive(SkeletonClearedEvent{});
   SetStance(mStance);
}

void Dock::Receive(const Engine::ComponentAddedEvent<SkeletonCollection>& evt)
{
   mSkeletons = evt.component;

   // Every stance has a parent
   for (const auto& entry : mSkeletons->parents)
   {
      mStances.push_back(entry.first);
   }
   SetStance(mStance);
}

void Dock::Receive(const SuspendEditingEvent&)
{
   if (!mSkeleton || !mSkeletons)
   {
      return;
   }

   // Not editing yet
   if (!mBonePos[0].text->IsBound())
   {
      return;
   }

   // Revert any pending edits so that they are not persisted.
   Skeleton::Stance& stance = mSkeleton->stances[mStance];
   {
      bool changed[4] = {false}; // position, rotation, scale, parent
      bool checked[4] = {false}; // position, rotation, scale, parent
      for (std::string it = mSkeletons->parents[mStance]; !it.empty(); it = mSkeletons->parents[it])
      {
         if (!checked[0] && mSkeleton->stances[it].positions.count(mBone) != 0)
         {
            changed[0] = mSkeleton->stances[it].positions[mBone] != stance.positions[mBone];
            checked[0] = true;
         }
         if (!checked[1] && mSkeleton->stances[it].rotations.count(mBone) != 0)
         {
            changed[1] = mSkeleton->stances[it].rotations[mBone] != stance.rotations[mBone];
            checked[1] = true;
         }
         if (!checked[2] && mSkeleton->stances[it].scales.count(mBone) != 0)
         {
            changed[2] = mSkeleton->stances[it].scales[mBone] != stance.scales[mBone];
            checked[2] = true;
         }
         if (!checked[3] && mSkeleton->stances[it].parents.count(mBone) != 0)
         {
            changed[3] = mSkeleton->stances[it].parents[mBone] != stance.parents[mBone];
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
}

void Dock::Receive(const ResumeEditingEvent&)
{
   if (!mSkeleton || !mSkeletons)
   {
      return;
   }

   Skeleton::Stance& stance = mSkeleton->stances[mStance];
   bool set[4] = {false}; // position, rotation, scale, parent
   for (std::string it = mStance; !it.empty(); it = mSkeletons->parents[it])
   {
      Skeleton::Stance& s = mSkeleton->stances[it];
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

   glm::vec3& position = stance.positions[mBone];
   glm::vec3& rotation = stance.rotations[mBone];
   glm::vec3& scale = stance.scales[mBone];

   mBonePos[0].text->Bind(&position.x);
   mBonePos[1].text->Bind(&position.y);
   mBonePos[2].text->Bind(&position.z);
   mBoneRot[0].text->Bind(&rotation.x);
   mBoneRot[1].text->Bind(&rotation.y);
   mBoneRot[2].text->Bind(&rotation.z);
   mBoneScl[0].text->Bind(&scale.x);
   mBoneScl[1].text->Bind(&scale.y);
   mBoneScl[2].text->Bind(&scale.z);

   mBonePos[0].scrubber->Bind(&position.x);
   mBonePos[1].scrubber->Bind(&position.y);
   mBonePos[2].scrubber->Bind(&position.z);
   mBoneRot[0].scrubber->Bind(&rotation.x);
   mBoneRot[1].scrubber->Bind(&rotation.y);
   mBoneRot[2].scrubber->Bind(&rotation.z);
   mBoneScl[0].scrubber->Bind(&scale.x);
   mBoneScl[1].scrubber->Bind(&scale.y);
   mBoneScl[2].scrubber->Bind(&scale.z);
}

///
///
///
void Dock::SetStance(const std::string& stance)
{
   mStance = stance;

   if (mSkeletons)
   {
      mSkeletons->stance = mStance;
   }

   SetBone(mBone);
}

void Dock::SetBone(const std::string& bone)
{
   if (!mSkeleton)
   {
      mBone = bone;
      return;
   }

   Receive(SuspendEditingEvent{});

   mBone = bone;

   // Update bone info
   mBoneName->SetText(mBone);

   Receive(ResumeEditingEvent{});
}

///
///
///
void Dock::NextBoneCommand::Do()
{
   size_t ndx = dock->mSkeleton->boneLookup[dock->mBone];
   if (ndx >= dock->mSkeleton->bones.size() - 1)
   {
      dock->SetBone(dock->mSkeleton->bones[0].name);
   }
   else
   {
      dock->SetBone(dock->mSkeleton->bones[ndx + 1].name);
   }
}

///
///
///
void Dock::NextBoneCommand::Undo()
{
   size_t ndx = dock->mSkeleton->boneLookup[dock->mBone];
   if (ndx == 0)
   {
      dock->SetBone(dock->mSkeleton->bones[dock->mSkeleton->bones.size() - 1].name);
   }
   else
   {
      dock->SetBone(dock->mSkeleton->bones[ndx - 1].name);
   }
}

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

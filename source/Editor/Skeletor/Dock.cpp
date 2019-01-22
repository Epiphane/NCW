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

using Bone = AnimatedSkeleton::Bone;
using Engine::UIElement;
using Engine::UIFrame;
using Engine::UIStackView;
using UI::Button;
using UI::RectFilled;

Dock::Dock(Engine::UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "SkeletorDock", glm::vec4(0.2, 0.2, 0.2, 1))
   , mBone(9)
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
         Bone& bone = mSkeleton->bones[mBone];

         // Update animations.
         for (AnimatedSkeleton::State& state : mSkeleton->states)
         {
            for (AnimatedSkeleton::Keyframe& keyframe : state.keyframes)
            {
               if (keyframe.positions[mBone] == bone.originalPosition)
               {
                  keyframe.positions[mBone] = bone.position;
               }
               if (keyframe.rotations[mBone] == bone.originalRotation)
               {
                  keyframe.rotations[mBone] = bone.rotation;
               }
            }
         }

         bone.originalPosition = bone.position;
         bone.originalRotation = bone.rotation;

         mpRoot->Emit<SkeletonModifiedEvent>(mSkeleton);
      };
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

   root->Subscribe<SkeletonLoadedEvent>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<AnimatedSkeleton>>(*this);
}

///
///
///
void Dock::Receive(const SkeletonLoadedEvent& evt)
{
   mSkeleton = evt.component;
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<AnimatedSkeleton>& evt)
{
   mSkeleton = evt.component;
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

   mBonePos[0].scrubber->Bind(&bone.position.x);
   mBonePos[1].scrubber->Bind(&bone.position.y);
   mBonePos[2].scrubber->Bind(&bone.position.z);
   mBoneRot[0].scrubber->Bind(&bone.rotation.x);
   mBoneRot[1].scrubber->Bind(&bone.rotation.y);
   mBoneRot[2].scrubber->Bind(&bone.rotation.z);
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

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

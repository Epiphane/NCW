// By Thomas Steinke

#include <fstream>
#include <RGBFileSystem/File.h>
#include <Engine/Core/Window.h>
#include <Engine/UI/UIStackView.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/TextButton.h>

#include "../Command/CommandStack.h"
#include "SkeletonList.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

using Engine::UIRoot;
using UI::RectFilled;
using UI::TextButton;

SkeletonList::SkeletonList(UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "AnimationStationSkeletonList", glm::vec4(0.2, 0.2, 0.2, 1))
   , mButtons{}
   , mSkeletons{}
   , mActive{0}
{
   RectFilled* foreground = Add<RectFilled>("AnimationStationSkeletonListFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainCenterTo(this);
   foreground->ConstrainDimensionsTo(this, -4);

   // Labels
   Engine::UIStackView* buttons = foreground->Add<Engine::UIStackView>("AnimationStationSkeletonListStackView");
   buttons->SetOffset(8.0);

   // Initialize 8 buttons in advance. I doubt we'll ever need this many but you never know.
   TextButton::Options buttonOptions;
   buttonOptions.size = 24;
   TextButton* prev = nullptr;
   for (size_t i = 0; i < 8; ++i)
   {
      buttonOptions.onClick = std::bind([this, i] {
         CommandStack::Instance().Do<SelectSkeletonCommand>(this, i);
      });
      TextButton* btn = buttons->Add<TextButton>(buttonOptions);
      btn->SetActive(false);
      if (!prev)
      {
         btn->ConstrainLeftAlignedTo(buttons, 2);
         btn->ConstrainWidthTo(buttons, -4);
         btn->ConstrainHeight(32);
      }
      else
      {
         btn->ConstrainDimensionsTo(prev);
         btn->ConstrainLeftAlignedTo(prev);
      }
      prev = btn;
      mButtons.push_back(btn);
   }

   buttons->ConstrainTopAlignedTo(foreground);
   buttons->ConstrainHorizontalCenterTo(foreground);
   buttons->ConstrainWidthTo(foreground, -12);

   root->Subscribe<Engine::ComponentAddedEvent<SimpleAnimationController>>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<Skeleton>>(*this);
   root->Subscribe<SkeletonClearedEvent>(*this);
}

void SkeletonList::Receive(const Engine::ComponentAddedEvent<SimpleAnimationController>& evt)
{
   mController = evt.component;
}

void SkeletonList::Receive(const Engine::ComponentAddedEvent<Skeleton>& evt)
{
   if (mSkeletons.size() >= mButtons.size())
   {
      LOG_ERROR("Too many skeletons! Not adding a new button because I'm laaaazy");
      return;
   }
   mButtons[mActive]->Unfocus();
   mActive = mSkeletons.size();
   mButtons[mActive]->SetActive(true);
   mButtons[mActive]->SetText(evt.component->name);
   mButtons[mActive]->Focus();

   mSkeletons.push_back(evt.component);
   mpRoot->Emit<SkeletonSelectedEvent>(mActive, evt.component);
}

void SkeletonList::Receive(const SkeletonClearedEvent&)
{
   mSkeletons.clear();
   for (const auto& button : mButtons)
   {
      button->SetActive(false);
   }
}

void SkeletonList::SelectSkeleton(size_t index)
{
   mButtons[mActive]->Unfocus();
   mActive = index;
   mButtons[mActive]->Focus();

   mpRoot->Emit<SkeletonSelectedEvent>(index, mSkeletons[index]);
}

void SkeletonList::ToggleSkeletonActive(size_t /*index*/)
{
   // TODO
}

void SkeletonList::SelectSkeletonCommand::Do()
{
   size_t prev = list->mActive;
   list->SelectSkeleton(index);
   index = prev;
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

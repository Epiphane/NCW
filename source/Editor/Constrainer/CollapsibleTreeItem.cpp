//
// CollapsibleTreeItem.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleTreeItem.h"

#include <Engine/UI/UITapGestureRecognizer.h>
#include <Shared/Helpers/Asset.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using UI::Image;
using UI::Text;
   
using Engine::UIConstraint;
using Engine::UIStackView;

CollapsibleTreeItem::CollapsibleTreeItem(Engine::UIRoot* root, Engine::UIElement* parent, const std::string &name, const std::string &title)
   : UIElement(root, parent, name)
   , mbExpanded(true)
{
   mArrow = Add<Image>(Image::Options{Asset::Image("EditorIcons.png"), "button_down"});
   mLabel = Add<Text>(Text::Options{title});
   mSelectableArea = Add<UIElement>();
   mSubElementStackView = Add<UIStackView>();
   
   mLabel->ConstrainWidthToContent();
   mLabel->ConstrainHeightToContent();

   mArrow->ConstrainWidthToContent();
   mArrow->ConstrainHeightToContent();
   
   mLabel->ConstrainToRightOf(mArrow, 5.0);
   mLabel->ConstrainVerticalCenterTo(mArrow);
   
   mSelectableArea->Contains(mLabel);
   mSelectableArea->Contains(mArrow);
   mSelectableArea->ConstrainLeftAlignedTo(this);
   mSelectableArea->ConstrainTopAlignedTo(this);
   
   mSubElementStackView->SetAlignItemsBy(UIStackView::Left);
   mSubElementStackView->ConstrainLeftAlignedTo(this, 20.0);
   mSubElementStackView->ConstrainBelow(mSelectableArea);
   mSubElementStackView->ConstrainBottomAlignedTo(this);
   
   Engine::GestureCallback callback = std::bind(&CollapsibleTreeItem::TapMeDaddy, this, std::placeholders::_1);
   mArrow->CreateAndAddGestureRecognizer<Engine::UITapGestureRecognizer>(callback);
   
   // Arrow starts hidden, unhides if this element gains children
   mArrow->SetActive(false);
}
   
void CollapsibleTreeItem::AddSubElement(std::unique_ptr<CollapsibleTreeItem> newSubElement)
{
   mSubElements.push_back(newSubElement.get());
   
   mSubElementStackView->AddChild(std::move(newSubElement));
   
   mArrow->SetActive(true);
}
   
void CollapsibleTreeItem::TapMeDaddy(const Engine::UIGestureRecognizer& rec) {
   if (rec.GetState() == Engine::UIGestureRecognizer::Ending) {
      mbExpanded = !mbExpanded;
      if (mbExpanded) {
         mArrow->SetImage("button_down");
         for (auto sub : mSubElements) {
            sub->SetActive(true);
         }
      }
      else
      {
         mArrow->SetImage("button_right");
         for (auto sub : mSubElements) {
            sub->SetActive(false);
         }
      }
   }
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

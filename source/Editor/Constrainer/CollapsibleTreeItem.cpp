//
// CollapsibleTreeItem.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleTreeItem.h"

#include "CollapsibleTreeView.h"

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
   , mbSelected(false)
{
   mArrow = Add<Image>(Image::Options{Asset::Image("EditorIcons.png"), "button_down"});
   mLabel = Add<Text>(Text::Options{title});
   mSelectableArea = Add<UIElement>();
   mSubElementStackView = Add<UIStackView>();
   mSelectedHighlight = Add<UI::RectFilled>("TreeItemHighlighter", glm::vec4(0.3, 0.3, 0.3, 1));
   
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
   
   mSelectedHighlight->ConstrainEqualBounds(mSelectableArea);
   mSelectedHighlight->ConstrainBehind(mArrow);
   mSelectedHighlight->ConstrainBehind(mLabel);
   
   mSubElementStackView->SetAlignItemsBy(UIStackView::Left);
   mSubElementStackView->ConstrainLeftAlignedTo(mLabel, 0.0);
   mSubElementStackView->ConstrainBelow(mSelectableArea);
   
   UIConstraint::Options weakSauce;
   weakSauce.priority = UIConstraint::HIGH_PRIORITY;
   mSubElementStackView->ConstrainBottomAlignedTo(this, 0.0, weakSauce);
   
   mStackViewHeightConstraint = mSubElementStackView->ConstrainHeight(0);
   mpRoot->RemoveConstraint(mStackViewHeightConstraint.GetName());
   
   Engine::GestureCallback expandCallback = std::bind(&CollapsibleTreeItem::TapMeDaddy, this, std::placeholders::_1);
   mArrow->CreateAndAddGestureRecognizer<Engine::UITapGestureRecognizer>(expandCallback);
   
   Engine::GestureCallback selectCallback = std::bind(&CollapsibleTreeItem::SelectItem, this, std::placeholders::_1);
   mLabel->CreateAndAddGestureRecognizer<Engine::UITapGestureRecognizer>(selectCallback);
   
   // Arrow starts hidden, unhides if this element gains children
   mArrow->SetActive(false);
   
   mSelectedHighlight->SetActive(false);
}

void CollapsibleTreeItem::AddSubElement(std::unique_ptr<CollapsibleTreeItem> newSubElement)
{
   mSubElements.push_back(newSubElement.get());
   
   mSubElementStackView->AddChild(std::move(newSubElement));
   
   mArrow->SetActive(true);
}

void CollapsibleTreeItem::SetActive(bool active)
{
   UIElement::SetActive(active);

   if (mActive) {
      if (mSubElements.empty()) {
         mArrow->SetActive(false);
      }

      if (!mbExpanded) {
         for (auto sub : mSubElements) {
            sub->SetActive(false);
         }
      }
      
      mSelectedHighlight->SetActive(mbSelected);
   }
}
   
void CollapsibleTreeItem::SetTreeView(CollapsibleTreeView* treeView)
{
   mTreeViewParent = treeView;
}
   
void CollapsibleTreeItem::SetHighlighted(bool bHighlighted)
{
   mbSelected = bHighlighted;
   
   mSelectedHighlight->SetActive(bHighlighted);
}

void CollapsibleTreeItem::TapMeDaddy(const Engine::UIGestureRecognizer& rec) {
   if (rec.GetState() == Engine::UIGestureRecognizer::Ending) {
      mbExpanded = !mbExpanded;
      if (mbExpanded) {
         mArrow->SetImage("button_down");
         for (auto sub : mSubElements) {
            sub->SetActive(true);
         }
         mpRoot->RemoveConstraint(mStackViewHeightConstraint.GetName());
      }
      else
      {
         mArrow->SetImage("button_right");
         for (auto sub : mSubElements) {
            sub->SetActive(false);
         }
         mpRoot->AddConstraint(mStackViewHeightConstraint);
      }
   }
}
   
void CollapsibleTreeItem::SelectItem(const Engine::UIGestureRecognizer& rec) {
   if (rec.GetState() == Engine::UIGestureRecognizer::Ending) {
      mTreeViewParent->ItemWasClicked(this);
   }
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

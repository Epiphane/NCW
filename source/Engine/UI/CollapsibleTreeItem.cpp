//
//  CollapsibleTreeItem.cpp
//  Engine
//
//  Created by Elliot Fiske on 6/12/19.
//

#include "CollapsibleTreeItem.h"

#include "UIClickGestureRecognizer.h"
#include "UIStackView.h"

#include <RGBBinding/ObservableBasicOperations.h>
#include <Shared/Helpers/Asset.h>

namespace CubeWorld
{
   
namespace Engine
{
   
using namespace Observables;
   
CollapsibleTreeItem::CollapsibleTreeItem(Engine::UIRoot* root, UIElement* parent, const std::string &name)
   : UIElement(root, parent, name)
   , mSubItemObservable(std::vector<CollapsibleTreeItem*>())
{
   mLabel = Add<Text>(Text::Options{}, mName + "Label");
   mSubItemStackView = Add<UIStackView>(mName + "SubItemParent");

   mSelectionToggle = Add<ToggleButtonVC>(mName + "SelectionToggle");
   mSelectedHighlight = Add<RectFilled>(mName + "Highlight", glm::vec4(0.3, 0.3, 0.3, 1));
   
   Image::Options offImage;
   offImage.filename = Asset::Image("EditorIcons.png");
   offImage.image = "button_right";

   Image::Options onImage;
   onImage.filename = Asset::Image("EditorIcons.png");
   onImage.image = "button_down";

   mExpandToggle = Add<ToggleButtonVC>(offImage, onImage, mName + "Toggle");

   CombineLatest(mActiveObservable, GetSelectionObservable()) >>
      OnMessage<std::tuple<bool, bool>>([&](auto activeAndSelected) {
         auto [bActive, bSelected] = activeAndSelected;
         mSelectedHighlight->SetActive(bActive && bSelected);
      }, mBag);
   
   mDataSink >>
      OnMessage<Data>([&](Data newData) {
         for (UIElement* subItem : mSubItems) {
            subItem->MarkForDeletion();
         }
         
         mSubItems.clear();
         
         mLabel->SetText(newData.title);

         for (const Data& newSubItemData : newData.children) {
            auto newItem = mSubItemStackView->Add<CollapsibleTreeItem>();
            newItem->GetDataSink().SendMessage(newSubItemData);
            
            // Indent sub-items
//            newItem->mExpandToggle->ConstrainLeftAlignedTo(mLabel, 0.0);
            
            mSubItems.push_back(newItem);
         }

         mSubItemObservable.SendMessage(mSubItems);
      }, mBag);

   CombineLatest(mSubItemObservable, mActiveObservable, mExpandToggle->GetToggleObservable()) >>
      OnMessage<std::tuple<std::vector<CollapsibleTreeItem*>, bool, bool>>([&](auto newState) {
         const auto& [subItems, bActive, bExpanded] = newState;

         for (CollapsibleTreeItem* subItem : subItems) {
            subItem->SetActive(bActive && bExpanded);
         }

         // If we have no subItems, there's no need to show the expansion toggle
         mExpandToggle->SetActive(bActive && !subItems.empty());
      }, mBag);

   CombineLatest(mSelectionToggle->GetToggleObservable(), mActiveObservable) >>
      OnMessage<std::tuple<bool, bool>>([&](auto newState) {
         const auto& [bSelected, bActive] = newState;
         mSelectedHighlight->SetActive(bSelected && bActive);
      }, mBag);

   mLabel->ConstrainWidthToContent();
   mLabel->ConstrainHeightToContent();
   mLabel->ConstrainToRightOf(mExpandToggle, 5.0);
   mLabel->ConstrainVerticalCenterTo(mExpandToggle);
   mLabel->ConstrainRightAlignedTo(this);
   mExpandToggle->ConstrainInFrontOf(mSelectedHighlight);
   mExpandToggle->ConstrainInFrontOf(mSelectionToggle);
   mExpandToggle->ConstrainWidth(20);
   mExpandToggle->ConstrainHeight(20);
   mExpandToggle->ConstrainTopAlignedTo(this);
   
   // Constrain toggle to the left side of the item, but allow it to move FURTHER IN
   //    in case it needs to be indented.
   UIConstraint::Options greaterThan;
   greaterThan.relationship = UIConstraint::GreaterThanOrEqual;
   mExpandToggle->ConstrainLeftAlignedTo(this, 0.0, greaterThan);

   mSelectedHighlight->ConstrainLeftAlignedTo(this);
   mSelectedHighlight->ConstrainTopAlignedTo(this);
   mSelectedHighlight->ConstrainRightAlignedTo(this);
   mSelectedHighlight->ConstrainHeight(30);
   mSelectionToggle->ConstrainEqualBounds(mSelectedHighlight);

   mSubItemStackView->SetAlignItemsBy(UIStackView::Left);
   mSubItemStackView->SetAlignItemsBy(UIStackView::Right);
   mSubItemStackView->ConstrainBelow(mSelectedHighlight);
   mSubItemStackView->ConstrainWidthTo(this);
   mSubItemStackView->ConstrainBottomAlignedTo(this);
   mSubItemStackView->ConstrainLeftAlignedTo(this);
   mSubItemStackView->ConstrainRightAlignedTo(this);
}

}; // namespace Engine
   
}; // namespace CubeWorld

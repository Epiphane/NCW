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
   
   Image::Options offImage{Asset::Image("EditorIcons.png"), "button_right"};
   Image::Options onImage{Asset::Image("EditorIcons.png"), "button_down"};
   mExpandToggle = Add<ToggleButtonVC>(offImage, onImage, mName + "Toggle");
   
   mSelectionToggle = Add<ToggleButtonVC>(mName + "SelectionToggle");
   mSelectedHighlight = Add<UI::RectFilled>(mName + "Highlight", glm::vec4(0.3, 0.3, 0.3, 1));
   
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
   
   mSelectedHighlight->Contains(mLabel);
   mSelectedHighlight->Contains(mExpandToggle);
   mSelectedHighlight->ConstrainLeftAlignedTo(this);
   mSelectedHighlight->ConstrainTopAlignedTo(this);
   mSelectedHighlight->ConstrainRightAlignedTo(this);
   mSelectedHighlight->ConstrainBehind(mExpandToggle);
   mSelectedHighlight->ConstrainBehind(mLabel);
   mSelectionToggle->ConstrainEqualBounds(mSelectedHighlight);

   mSubItemStackView->ConstrainBelow(mSelectedHighlight);
   mSubItemStackView->ConstrainWidthTo(this);
   mSubItemStackView->ConstrainBottomAlignedTo(this);
}

}; // namespace Engine
   
}; // namespace CubeWorld

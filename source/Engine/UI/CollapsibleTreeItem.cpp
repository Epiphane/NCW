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
   
CollapsibleTreeItem::CollapsibleTreeItem(Engine::UIRoot* root, UIElement* parent, const std::string &title, const std::string &name)
   : UIElement(root, parent, name)
{
   mLabel = Add<Text>(Text::Options{title}, mName + "Label");
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
   
   auto childrenChanged = mChildDataObservable >>
      OnMessage<std::vector<std::string>*>([&](auto* childTitles) {
         for (UIElement* subItem : mSubItems) {
            subItem->MarkForDeletion();
         }
         
         mSubItems.clear();
         
         for (const std::string& newTitle : *childTitles) {
            auto newItem = mSubItemStackView->Add<CollapsibleTreeItem>(newTitle);
            mSubItems.push_back(newItem);
            mChildItemObservable.SendMessage(newItem);
         }
      }, mBag);
   
//   CombineLatest(childrenChanged, mActiveObservable, mExpandToggle->GetToggleObservable()) >>
   
   
   // This code covers the case where you add a new subItem to me when I'm collapsed,
   //    AND hiding items when I get collapsed.
   // TODO-EF: jk it doesn't. Fix me plz.
//   CombineLatest(mChildItemObservable, mActiveObservable) >>
//      OnMessage<std::tuple<UIElement*, bool>>([&](auto activeAndNewChild) {
//         auto [newChild, bActive] = activeAndNewChild;
//         newChild->SetActive(bActive);
//      }, mBag);
//
//   CombineLatest(mChildItemObservable, mExpandToggle->GetToggleObservable()) >>
//      OnMessage<std::tuple<UIElement*, bool>>([&](auto newChildAndExpanded) {
//         auto [newChild, bExpanded] = newChildAndExpanded;
//         newChild->SetActive(bExpanded);
//      }, mBag);
   
   
   mLabel->SetText(title);
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
   
   mSubItemStackView->ConstrainBelow(mSelectedHighlight);
   mSubItemStackView->ConstrainWidthTo(this);
   mSubItemStackView->ConstrainBottomAlignedTo(this);
}
 
void CollapsibleTreeItem::SetActive(bool active)
{
   UIElement::SetActive(active);

   if (mActive) {
      // If no children, hide expansion toggle
      if (mSubItems.empty()) {
         mExpandToggle->SetActive(false);
      }
   }
}

}; // namespace Engine
   
}; // namespace CubeWorld

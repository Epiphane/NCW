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
   , mbExpanded(true)
   , mbSelected(false)
{
   mLabel = Add<Text>(Text::Options{title}, mName + "Label");
   mSubItemStackView = Add<UIStackView>(mName + "SubItemParent");
   mSelectedHighlight = Add<UI::RectFilled>(mName + "Highlight", glm::vec4(0.3, 0.3, 0.3, 1));
   
   Image::Options offImage{Asset::Image("EditorIcons.png"), "button_right"};
   Image::Options onImage{Asset::Image("EditorIcons.png"), "button_down"};
   mExpandToggle = Add<ToggleButtonVC>(offImage, onImage, mName + "Toggle");
   
   mExpandToggle->GetToggleObservable() >>
      OnMessage<bool>([&](bool expanded) {
         mbExpanded = expanded;
         for (UIElement* subItem : mSubItems) {
            subItem->SetActive(expanded);
         }
      }, mBag);
   
   // TODO-EF: I could really just turn mSelectedHighlight into a ToggleButton. That might be neat.
   auto clickToSelect = mSelectedHighlight->CreateAndAddGestureRecognizer<UIClickGestureRecognizer>();
   clickToSelect->OnClick() >>
      Map<UIGestureRecognizer::Message_GestureState, bool>([&](auto gesture) -> bool {
         return !mbSelected;
      }) >> 
      mSelectionObservable;
   
   mSelectionObservable >>
      StartWith(false) >>
      RemoveDuplicates() >>
      OnMessage<bool>([&](bool selected) {
         mSelectedHighlight->SetActive(selected);
         mbSelected = selected;
      }, mBag);
   
   mChildDataObservable >>
      OnMessage<std::vector<std::string>*>([&](std::vector<std::string>* childTitles) {
         for (UIElement* subItem : mSubItems) {
            subItem->MarkForDeletion();
         }
         
         mSubItems.clear();
         
         for (const std::string& newTitle : *childTitles) {
            auto newItem = mSubItemStackView->Add<CollapsibleTreeItem>(newTitle);
            mSubItems.push_back(newItem);
         }
      }, mBag);
   
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

      if (!mbExpanded) {
         for (UIElement* sub : mSubItems) {
            sub->SetActive(false);
         }
      }
      
      mSelectedHighlight->SetActive(mbSelected);
   }
}

}; // namespace Engine
   
}; // namespace CubeWorld

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
   mLabel = Add<Text>(Text::Options{title}, name + "Label");
   mSubElementStackView = Add<UIStackView>(name + "SubItemParent");
   mSelectedHighlight = Add<UI::RectFilled>(name + "Highlight", glm::vec4(0.3, 0.3, 0.3, 1));
   
   Image::Options offImage{Asset::Image("EditorIcons.png"), "button_right"};
   Image::Options onImage{Asset::Image("EditorIcons.png"), "button_down"};
   mToggle = Add<ToggleButtonVC>(offImage, onImage, name + "Toggle");
   
   mToggle->OnToggled() >>
      OnMessage<bool>([&](bool expanded) {
         for (UIElement* sub : mSubElements) {
            sub->SetActive(expanded);
            mbExpanded = expanded;
         }
      }, mBag);
   
   auto clickToSelect = mSelectedHighlight->CreateAndAddGestureRecognizer<UIClickGestureRecognizer>();
   clickToSelect->OnClick() >>
      OnMessage<UIGestureRecognizer::Message_GestureState>([&](auto m) {
         mSelections.SendMessage(m);
      }, mBag);
   
   mLabel->ConstrainWidthToContent();
   mLabel->ConstrainHeightToContent();
   mLabel->ConstrainToRightOf(mToggle, 5.0);
   mLabel->ConstrainVerticalCenterTo(mToggle);
   
   mSelectedHighlight->Contains(mLabel);
   mSelectedHighlight->Contains(mToggle);
   mSelectedHighlight->ConstrainLeftAlignedTo(this);
   mSelectedHighlight->ConstrainTopAlignedTo(this);
   mSelectedHighlight->ConstrainRightAlignedTo(this);
   mSelectedHighlight->ConstrainBehind(mToggle);
   mSelectedHighlight->ConstrainBehind(mLabel);
   
   mSubElementStackView->ConstrainBelow(mSelectedHighlight);
   mSubElementStackView->ConstrainWidthTo(this);
   mSubElementStackView->ConstrainBottomAlignedTo(this);
}
 
void CollapsibleTreeItem::SetActive(bool active)
{
   UIElement::SetActive(active);

   if (mActive) {
      if (mSubElements.empty()) {
         mToggle->SetActive(false);
      }

      if (!mbExpanded) {
         for (UIElement* sub : mSubElements) {
            sub->SetActive(false);
         }
      }
      
      mSelectedHighlight->SetActive(mbSelected);
   }
}

}; // namespace Engine
   
}; // namespace CubeWorld

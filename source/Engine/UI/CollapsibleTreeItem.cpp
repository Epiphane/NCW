//
//  CollapsibleTreeItem.cpp
//  Engine
//
//  Created by Elliot Fiske on 6/12/19.
//

#include "CollapsibleTreeItem.h"

#include "UIClickGestureRecognizer.h"
#include "UIStackView.h"

#include "UIRoot.h"

#include <RGBBinding/ObservableBasicOperations.h>

namespace CubeWorld
{
   
namespace Engine
{
   
using namespace Observables;
   
CollapsibleTreeItem::CollapsibleTreeItem(Engine::UIRoot* root, UIElement* parent, const std::string &title, const std::string &name)
   : CollapsibleContent(root, parent, name)
   , mbExpanded(true)
   , mbSelected(false)
{
   mLabel = Add<Text>(Text::Options{title}, name + "Label");
   mSubElementStackView = AddContent<UIStackView>(name + "SubItemParent");
   mSelectedHighlight = Add<UI::RectFilled>(name + "Highlight", glm::vec4(0.3, 0.3, 0.3, 1));
   
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
   
   mContentParent->ConstrainBelow(mSelectedHighlight);
   mContentParent->ConstrainLeftAlignedTo(this);
   mContentParent->ConstrainRightAlignedTo(this);
   mContentParent->ConstrainBottomAlignedTo(this);
}
 
void CollapsibleTreeItem::SetActive(bool active)
{
   UIElement::SetActive(active);

   if (mActive) {
      if (mSubElements.empty()) {
         mToggle->SetActive(false);
      }

      if (!mbExpanded) {
         
      }
      
      mSelectedHighlight->SetActive(mbSelected);
   }
}
   
void CollapsibleTreeItem::SetHighlighted(bool bHighlighted)
{
   mbSelected = bHighlighted;
   
   mSelectedHighlight->SetActive(bHighlighted);
}

}; // namespace Engine
   
}; // namespace CubeWorld

//
// CollapsibleTreeItem.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleTreeItem.h"

#include "CollapsibleTreeVC.h"

#include <Engine/UI/UIClickGestureRecognizer.h>
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
   mLabel = Add<Text>(Text::Options{title}, name + "Label");
   mSelectableArea = Add<UIElement>(name + "SelectableArea");
   mSubElementStackView = Add<UIStackView>();
   mSelectedHighlight = Add<UI::RectFilled>(name + "Highlighter", glm::vec4(0.3, 0.3, 0.3, 1));
   
   mLabel->ConstrainWidthToContent();
   mLabel->ConstrainHeightToContent();
   
   mLabel->ConstrainToRightOf(mToggle, 5.0);
   mLabel->ConstrainVerticalCenterTo(mToggle);
   
   mSelectableArea->Contains(mLabel);
   mSelectableArea->Contains(mToggle);
   mSelectableArea->ConstrainLeftAlignedTo(this);
   mSelectableArea->ConstrainTopAlignedTo(this);
   
   mSelectedHighlight->ConstrainEqualBounds(mSelectableArea);
   mSelectedHighlight->ConstrainBehind(mToggle);
   mSelectedHighlight->ConstrainBehind(mLabel);
   
   mSubElementStackView->SetAlignItemsBy(UIStackView::Left);
   mSubElementStackView->ConstrainLeftAlignedTo(mLabel, 0.0);
   mSubElementStackView->ConstrainBelow(mSelectableArea);
}

void CollapsibleTreeItem::AddSubElement(std::unique_ptr<CollapsibleTreeItem> newSubElement)
{
   mSubElements.push_back(newSubElement.get());
   
   mSubElementStackView->AddChild(std::move(newSubElement));
   
   mToggle->SetActive(true);
}
 
void CollapsibleTreeItem::SetActive(bool active)
{
   UIElement::SetActive(active);

   if (mActive) {
      if (mSubElements.empty()) {
         mToggle->SetActive(false);
      }

      if (!mbExpanded) {
         for (auto sub : mSubElements) {
            sub->SetActive(false);
         }
      }
      
      mSelectedHighlight->SetActive(mbSelected);
   }
}
   
void CollapsibleTreeItem::SetHighlighted(bool bHighlighted)
{
   mbSelected = bHighlighted;
   
   mSelectedHighlight->SetActive(bHighlighted);
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

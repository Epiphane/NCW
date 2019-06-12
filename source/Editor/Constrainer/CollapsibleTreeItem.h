//
// CollapsibleTreeItem.h
//
// A toggle button with a label, with children beneath it. Can click to expand to see this
//    element's children in the hierarchy.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/CollapsibleContent.h>
#include <Engine/UI/UIStackView.h>
#include <Engine/UI/UIClickGestureRecognizer.h>

#include <Shared/UI/RectFilled.h>
#include <Shared/UI/Image.h>
#include <Shared/UI/Text.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{
   
class CollapsibleTreeItem : public Engine::CollapsibleContent
{
public:
   CollapsibleTreeItem(Engine::UIRoot* root, UIElement* parent, const std::string& title, const std::string& name = "");
   
   // Add a new element below this one in the tree heirarchy
   void AddSubElement(std::unique_ptr<CollapsibleTreeItem> newSubElement);

   void SetActive(bool active) override;
   
   // When you click an item, it will gain a selection highlight to show it's the current selected item
   void SetHighlighted(bool bHighlighted);

private:
   Engine::UIConstraint mStackViewHeightConstraint;
   
   UI::Text*  mLabel;
   UIElement* mSelectableArea;
   UI::RectFilled* mSelectedHighlight;
   
   Engine::UIStackView* mSubElementStackView;
   
   // List of elements beneath this one in the heirarchy
   std::vector<CollapsibleTreeItem*> mSubElements;
   
   // If true, show the elements beneath this in the hierarchy
   bool mbExpanded;
   
   // If true, show element highlight
   bool mbSelected;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld


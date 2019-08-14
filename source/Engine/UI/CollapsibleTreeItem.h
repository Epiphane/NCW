//
// CollapsibleTreeItem.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "UIElement.h"
#include "ToggleButtonVC.h"

#include <RGBBinding/Observable.h>

#include <Shared/UI/Text.h>
#include <Shared/UI/RectFilled.h>
#include <Engine/UI/UIStackView.h>

namespace CubeWorld
{
   
namespace Engine
{
   
using UI::Text;
using UI::RectFilled;

class CollapsibleTreeItem : public UIElement
{
public:
   struct Data {
      std::string title;
      std::vector<Data> children;
   };
   
   CollapsibleTreeItem(UIRoot* root, UIElement* parent, const std::string& name = "");
   virtual ~CollapsibleTreeItem() {}
   
   Observables::Observable<bool>& GetSelectionObservable() { return mSelectionToggle->GetToggleObservable(); }
   Observables::Observable<Data>& GetDataSink() { return mDataSink; }
   
   const std::vector<CollapsibleTreeItem*>& GetSubItems() const { return mSubItems; }
   const std::string& GetTitle() const { return mLabel->GetText(); }
   
private:
   // Send collapsible item data in through here, and this element will react by remaking its subItems
   //    with the given strings as titles, and setting its own title to the appropriate string.
   Observables::Observable<Data> mDataSink;
   
   // Newly created subItems will pop out through here.
   Observables::Observable<std::vector<CollapsibleTreeItem*>> mSubItemObservable;
   
   Text* mLabel;
   ToggleButtonVC* mExpandToggle;
   ToggleButtonVC* mSelectionToggle;
   RectFilled* mSelectedHighlight;
   
   // Parent to the collapsible items beneath me
   UIStackView* mSubItemStackView;
   
   // List of elements beneath this one in the heirarchy
   std::vector<CollapsibleTreeItem*> mSubItems;
};
   
}; // namespace Engine
   
}; // namespace CubeWorld

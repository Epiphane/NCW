//
// CollapsibleTreeItem.h
//
// This file created by the ELLIOT FISKE gang
//

#include "UIElement.h"
#include "ToggleButtonVC.h"

#include <RGBBinding/Observable.h>

#include <Shared/UI/Text.h>
#include <Shared/UI/RectFilled.h>

namespace CubeWorld
{
   
namespace Engine
{
   
using UI::Text;
using UI::RectFilled;

class CollapsibleTreeItem : public UIElement
{
public:
   CollapsibleTreeItem(UIRoot* root, UIElement* parent, const std::string& title, const std::string& name = "");
   virtual ~CollapsibleTreeItem() {}
   
   void SetActive(bool active) override;
   
   Observables::Observable<bool>& GetSelectionObservable() { return mSelectionToggle->GetToggleObservable(); }
   Observables::Observable<std::vector<std::string>*>& GetChildDataObservable() { return mChildDataObservable; }
   
private:
   // Send string vectors in through here, and this element will react by remaking its subItems
   //    with the given strings as titles.
   Observables::Observable<std::vector<std::string>*> mChildDataObservable;
   
   // Newly created subItems will pop out through here.
   Observables::Observable<UIElement*> mChildItemObservable;
   
   Text* mLabel;
   ToggleButtonVC* mExpandToggle;
   ToggleButtonVC* mSelectionToggle;
   RectFilled* mSelectedHighlight;
   
   // Parent to the collapsible items beneath me
   Engine::UIElement* mSubItemStackView;
   
   // List of elements beneath this one in the heirarchy
   std::vector<CollapsibleTreeItem*> mSubItems;
};
   
}; // namespace Engine
   
}; // namespace CubeWorld

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
   
private:
   DECLARE_OBSERVABLE(UIGestureRecognizer::Message_GestureState, mSelections, OnSelected);
   
   Text* mLabel;
   ToggleButtonVC* mToggle;
   RectFilled* mSelectedHighlight;
   
   // Parent to the collapsible items beneath me
   Engine::UIElement* mSubElementStackView;
   
   // List of elements beneath this one in the heirarchy
   std::vector<CollapsibleTreeItem*> mSubElements;
   
   // If true, show the elements beneath this in the heirarchy
   bool mbExpanded;
   
   // If true, show element highlight
   bool mbSelected;
};
   
}; // namespace Engine
   
}; // namespace CubeWorld

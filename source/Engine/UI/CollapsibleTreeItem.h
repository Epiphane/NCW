//
// CollapsibleTreeItem.h
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleContent.h"

#include <RGBBinding/Observable.h>

#include <Shared/UI/Text.h>
#include <Shared/UI/RectFilled.h>

namespace CubeWorld
{
   
namespace Engine
{
   
using UI::Text;
using UI::RectFilled;

class CollapsibleTreeItem : public CollapsibleContent
{
public:
   CollapsibleTreeItem(Engine::UIRoot* root, Engine::UIElement* parent, const std::string& title, const std::string& name = "");
   virtual ~CollapsibleTreeItem() {}
   
   void SetActive(bool active) override;
   
   // When you click an item, it will gain a selection highlight to show it's the current selected item
   void SetHighlighted(bool bHighlighted);
   
private:
   DECLARE_OBSERVABLE(UIGestureRecognizer::Message_GestureState, mSelections, OnSelected);
   
   Text* mLabel;
   RectFilled* mSelectedHighlight;
   
   Engine::UIElement* mSubElementStackView;
   
   // List of elements beneath this one in the heirarchy
   std::vector<CollapsibleTreeItem*> mSubElements;
   
   // If true, show the elements beneath this in the hierarchy
   bool mbExpanded;
   
   // If true, show element highlight
   bool mbSelected;
};
   
}; // namespace Engine
   
}; // namespace CubeWorld

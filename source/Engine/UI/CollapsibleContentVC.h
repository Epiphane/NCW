//
// CollapsibleContentVC.h
//
// This describes an element with a label and a ToggleButtonVC that
//    you can toggle to collapse/expand content beneath the label.
//
// This file created by the ELLIOT FISKE gang
//

#include "ToggleButtonVC.h"

namespace CubeWorld
{

namespace Engine
{

class CollapsibleContentVC : public UIElement
{
public:
   CollapsibleContentVC(UIRoot* root, UIElement* parent, const std::string &name);

private:
   void ExpandedStateChanged(bool bIsExpanded);
   
   // Constraint that sets mContent.height = 0. Disabled if content is expanded.
   UIConstraint mCollapseContent;

   ToggleButtonVC* mToggle;
   UIElement* mContent;
};

} // namespace Engine

} // namespace CubeWorld



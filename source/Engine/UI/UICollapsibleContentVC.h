//
// UICollapsibleContentVC.h
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

class UICollapsibleContentVC : public UIElement
{
public:
   UICollapsibleContentVC(UIRoot* root, UIElement* parent, const std::string &name);

private:
   void ExpandedStateChanged(bool bIsExpanded);
   
   // Constraint that sets mContent.height = 0. Disabled if content is expanded.
   UIConstraint mCollapseContent;
   
   // If true, the content is shown.
   Observable<bool> mIsExpanded;

   ToggleButtonVC* mToggle;
   UIElement* mContent;
};

} // namespace Engine

} // namespace CubeWorld



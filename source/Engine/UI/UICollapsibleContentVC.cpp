//
// UICollapsibleContentVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "UICollapsibleContentVC.h"

#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{

UICollapsibleContentVC::UICollapsibleContentVC(UIRoot* root, UIElement* parent, const std::string &name)
      : UIElement(root, parent, name)
{
   Observable<bool>& fart = mIsExpanded;
   mToggle = Add<ToggleButtonVC>(fart, name + "Toggle");
   mContent = Add<UIElement>(name + "Content");
   
   mCollapseContent = mContent->ConstrainHeight(0);
   mpRoot->RemoveConstraint(mCollapseContent.GetName());

   mIsExpanded.AddCallback(std::bind(&UICollapsibleContentVC::ExpandedStateChanged, this, std::placeholders::_1));
}
   
void UICollapsibleContentVC::ExpandedStateChanged(bool bIsExpanded)
{
   if (bIsExpanded) {
      mpRoot->RemoveConstraint(mCollapseContent.GetName());
   }
   else
   {
      mpRoot->AddConstraint(mCollapseContent);
   }
}

} // namespace Engine

} // namespace CubeWorld

//
// CollapsibleContentVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleContentVC.h"

#include "UIRoot.h"

#include <Shared/Helpers/Asset.h>

namespace CubeWorld
{

namespace Engine
{

CollapsibleContentVC::CollapsibleContentVC(UIRoot* root, UIElement* parent, const std::string &name)
      : UIElement(root, parent, name)
{
//   mIsExpanded = std::make_shared<Observable<bool>>();
   
//   Image::Options offImage{Asset::Image("EditorIcons.png"), "button_right"};
//   Image::Options onImage{Asset::Image("EditorIcons.png"), "button_down"};
//   mToggle = Add<ToggleButtonVC>(mIsExpanded, offImage, onImage, name + "Toggle");
   
   mContent = Add<UIElement>(name + "Content");
   mCollapseContent = mContent->ConstrainHeight(0);
   mpRoot->RemoveConstraint(mCollapseContent.GetName());

//   mIsExpanded->AddCallback(std::bind(&CollapsibleContentVC::ExpandedStateChanged, this, std::placeholders::_1));
}
   
void CollapsibleContentVC::ExpandedStateChanged(bool bIsExpanded)
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

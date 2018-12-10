//
//  UIElementDesignerWrapper.cpp
//  Editor
//
//  Created by Elliot Fiske on 12/5/18.
//

#include "UIElementDesignerWrapper.hpp"

namespace CubeWorld
{

namespace Editor
{

UIElementDesignerWrapper::UIElementDesignerWrapper(Engine::UIRoot* root, UIElement* parent, const std::string &name, UIElement* mWrappedElement)
      : UIElement(root, parent, name)
      , mWrappedElement(mWrappedElement)
{
   std::string overlayName = mWrappedElement->GetName();
   overlayName.append("_EditorOverlay");

   mElementOverlay = Add<RectFilled>(overlayName, glm::vec4(1, 0, 0, 1));
}

} // Editor

} // CubeWorld



//
//  UIElementDesignerWrapper.cpp
//  Editor
//
//  Created by Elliot Fiske on 12/5/18.
//

#include "UIElementDesignerWrapper.h"

#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

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

//   mElementOverlay = Add<RectFilled>(overlayName, glm::vec4(1, 1, 0, 1));
//
//   ConstrainEqualBounds(mElementOverlay);
//   mElementOverlay->ConstrainEqualBounds(mWrappedElement);

   mWrappedElement->ConstrainEqualBounds(this);

//   mElementOverlay->SetActive(false);
}

UIElement::Action UIElementDesignerWrapper::MouseClick(const MouseClickEvent &event)
{
   if (!ContainsPoint(event.x, event.y)) {
      return Unhandled;
   }

   std::list<Engine::UIContextMenu::Choice> choices = {
         {mWrappedElement->GetName(), std::bind(&UIElementDesignerWrapper::TestButton, this)},
         {"Test THIS out", std::bind(&UIElementDesignerWrapper::TestButton, this)},
         {"Test THIS out", std::bind(&UIElementDesignerWrapper::TestButton, this)}
   };

   mpRoot->CreateUIContextMenu(event.x, event.y, choices);

   LogDebugInfo();

   return Handled;
}

void UIElementDesignerWrapper::TestButton()
{

}

} // Editor

} // CubeWorld



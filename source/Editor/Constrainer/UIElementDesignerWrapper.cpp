//
//  UIElementDesignerWrapper.cpp
//  Editor
//
//  Created by Elliot Fiske on 12/5/18.
//

#include "UIElementDesignerWrapper.h"

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

   mElementOverlay = Add<RectFilled>(overlayName, glm::vec4(1, 0, 0, 1));

   mElementOverlay->ConstrainInFrontOfAllDescendants(mWrappedElement);

//   mElementOverlay->SetActive(false);
}

UIElement::Action UIElementDesignerWrapper::MouseClick(const MouseClickEvent &event)
{
   std::list<Engine::UIContextMenu::Choice> choices = {
         {"Test this out", std::bind(&UIElementDesignerWrapper::TestButton, this)},
         {"Test THIS out", std::bind(&UIElementDesignerWrapper::TestButton, this)},
         {"Test THIS out", std::bind(&UIElementDesignerWrapper::TestButton, this)}
   };

   mpRoot->CreateUIContextMenu(event.x, event.y, choices);

   return Handled;
}

void UIElementDesignerWrapper::TestButton()
{

}

} // Editor

} // CubeWorld



//
//  UIElementDesignerWrapper.hpp
//  Editor
//
//  Wraps around a UIElement and allows us to edit it in the
//    graphical editor.
//
//  Created by Elliot Fiske on 12/5/18.
//

#pragma once

#include <Engine/UI/UIElement.h>
#include <Shared/UI/RectFilled.h>

namespace CubeWorld
{
   
namespace Editor
{

using UI::RectFilled;
using Engine::UIElement;

class UIElementDesignerWrapper : public UIElement
{
public:
   UIElementDesignerWrapper(Engine::UIRoot* root, UIElement* parent, const std::string &name, UIElement* mWrappedElement);

private:
   UIElement*  mWrappedElement;
   RectFilled* mElementOverlay;
};
   
}  // Editor
   
}  // CubeWorld

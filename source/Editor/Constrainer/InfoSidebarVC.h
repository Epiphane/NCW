//
// InfoSidebarVC.h
//
// When the user selects a UIElement in the editor, this VC will display the name, frame, and properties
//    of the element. It will also have a list of constraints affecting the element.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElement.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::UIElement;

class InfoSidebarVC : public UIElement
{
public:
   InfoSidebarVC(Engine::UIRoot* root, UIElement* parent, const std::string &name);

};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld



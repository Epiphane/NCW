//
// InfoSidebarVC.h
//
// When the user selects a UIElementDep in the editor, this VC will display the name, frame, and properties
//    of the element. It will also have a list of constraints affecting the element.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElementDep.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::UIElementDep;

class InfoSidebarVC : public UIElementDep
{
public:
   InfoSidebarVC(Engine::UIRootDep* root, UIElementDep* parent, const std::string &name);

};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

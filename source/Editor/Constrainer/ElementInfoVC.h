//
// ElementInfoVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElementDep.h>
#include <Shared/UI/Text.h>

namespace CubeWorld
{

namespace Constrainer
{

using Engine::UIElementDep;
using UI::Text;

class ElementInfoVC : public UIElementDep
{
public:
   ElementInfoVC(Engine::UIRootDep* root, UIElementDep* parent, const std::string &name);

private:

};

} // namespace Constrainer

} // namespace CubeWorld

//
// ElementInfoVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElement.h>
#include <Shared/UI/Text.h>

namespace CubeWorld
{

namespace Constrainer
{

using Engine::UIElement;
using UI::Text;

class ElementInfoVC : public UIElement
{
public:
   ElementInfoVC(Engine::UIRoot* root, UIElement* parent, const std::string &name);

private:

};

} // namespace Constrainer

} // namespace CubeWorld

//
// ElementListSidebar.h
//
// Sidebar for the Constrainer that holds a list of the elements in the
//    current scene, in their hierarchy. You can click the arrow next
//    to an element to collapse it.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIRoot.h>
#include <Engine/UI/UIStackView.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::UIStackView;

class ElementListSidebar : public Engine::UIElement
{
public:
   ElementListSidebar(Engine::UIRoot* root, UIElement* parent);

private:
   UIStackView *mStackView;

};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

//
// ElementListItem.h
//
// An arrow next to an element name. Can click to expand to see this
//    element's children in the hierarchy.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UITapGestureRecognizer.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/Image.h>
#include <Shared/UI/Text.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

class ElementListItem : public Engine::UIElement
{
public:
   ElementListItem(Engine::UIRoot* root, UIElement* parent, const std::string &name);

private:
   void TapMeDaddy(const Engine::UIGestureRecognizer& rec);
   
   UI::Image* mArrow;
   UI::Text*  mLabel;
   
   // If true, show the elements beneath this in the hierarchy
   bool mbExpanded;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld


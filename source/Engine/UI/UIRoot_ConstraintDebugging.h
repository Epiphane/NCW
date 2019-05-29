//
// UIRoot_ConstraintDebugging.h
//
// UIElement that covers the entire UIRoot. If active, it will capture all mouse events
//    and use them to show you the bounds of hovered elements.
//
// Eventually this will show tooltips + represent constraints graphically, but for now
//    it just prints to the console.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElement.h>

namespace CubeWorld
{

namespace Engine
{

class UIRoot_ConstraintDebugging final : public UIElement
{
public:
   UIRoot_ConstraintDebugging(UIRoot* root, UIElement* parent, const std::string &name);

   Action MouseMove(const MouseMoveEvent& evt) override;
   Action MouseDown(const MouseDownEvent& evt) override;

private:
   // Pink rectangle that will highlight elements when you debug constraints
   UIElement* mConstraintDebugHighlight;

   // Label that goes over mConstraintDebugHighlight and tells you how many UIElements are under your mouses
   UIElement* mConstraintDebugLabel;
   UIElement* mConstraintDebugLabelBG;
};

} // namespace Engine

} // namespace CubeWorld

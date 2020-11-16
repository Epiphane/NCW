//
// UIRoot_ConstraintDebugging.h
//
// UIElementDep that covers the entire UIRootDep. If active, it will capture all mouse events
//    and use them to show you the bounds of hovered elements.
//
// Eventually this will show tooltips + represent constraints graphically, but for now
//    it just prints to the console.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElementDep.h>

namespace CubeWorld
{

namespace Engine
{

class UIRoot_ConstraintDebugging final : public UIElementDep
{
public:
   UIRoot_ConstraintDebugging(UIRootDep* root, UIElementDep* parent, const std::string &name);

   Action MouseMove(const MouseMoveEvent& evt) override;
   Action MouseDown(const MouseDownEvent& evt) override;

private:
   // Pink rectangle that will highlight elements when you debug constraints
   UIElementDep* mConstraintDebugHighlight;

   // Label that goes over mConstraintDebugHighlight and tells you how many UIElements are under your mouses
   UIElementDep* mConstraintDebugLabel;
   UIElementDep* mConstraintDebugLabelBG;
};

} // namespace Engine

} // namespace CubeWorld

//
// ButtonVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class ButtonVC : public UIElement
{
public:
   ButtonVC(UIRoot* root, UIElement* parent, const std::string &name);

private:
   enum State { NORMAL, HOVER, PRESS };
   void SetState(State state);
   
   void TapHandler(const Engine::UIGestureRecognizer& rec);

   // Visual state change callbacks
   std::function<void(ButtonVC*)> mIdleCallback;
   std::function<void(ButtonVC*)> mHoverCallback;
   std::function<void(ButtonVC*)> mPressCallback;
   
   // Callback for completed click
   std::function<void(void)> mClickCallback;
   
   State mState;
};

}; // namespace Engine

}; // namespace CubeWorld


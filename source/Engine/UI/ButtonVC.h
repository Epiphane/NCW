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

   Action MouseMove(const MouseMoveEvent &evt) override;

private:
   enum State { NORMAL, HOVER, PRESS };
   void SetState(State state);

   void TapHandler(const Engine::UIGestureRecognizer& rec);
   
   // Callback for completed click
   std::function<void(void)> mClickCallback;

   bool mIsHovered; ///< True if the mouse is currently over us
   State mState;
};

}; // namespace Engine

}; // namespace CubeWorld

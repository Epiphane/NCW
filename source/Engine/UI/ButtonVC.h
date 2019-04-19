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
   
   void SetCallback(std::function<void(void)> callback);
   
protected:
   virtual void OnClick();
   
private:
   enum State { NORMAL, HOVER, PRESS };
   void SetState(State state);

   void TapHandler(const Engine::UIGestureRecognizer& rec);
   
   std::function<void(void)> mClickCallback;

   State mState;
};

}; // namespace Engine

}; // namespace CubeWorld

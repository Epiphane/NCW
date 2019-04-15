//
// ButtonVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ButtonVC.h"

#include "UITapGestureRecognizer.h"

namespace CubeWorld
{

namespace Engine
{

ButtonVC::ButtonVC(UIRoot* root, UIElement* parent, const std::string &name)
   : UIElement(root, parent, name)
{
   Engine::GestureCallback tapCallback = std::bind(&ButtonVC::TapHandler, this, std::placeholders::_1);
   CreateAndAddGestureRecognizer<UITapGestureRecognizer>(tapCallback);
}
   
void ButtonVC::TapHandler(const Engine::UIGestureRecognizer& rec)
{
   switch (rec.GetState()) {
      case Engine::UIGestureRecognizer::Starting:
         break;
      case Engine::UIGestureRecognizer::Possible:
      case Engine::UIGestureRecognizer::Cancelled:
         break;
      case Engine::UIGestureRecognizer::Ending:
         break;
      case Engine::UIGestureRecognizer::Happening:
         // Do nothing, we're already depressed
         break;
   }
}

void ButtonVC::SetState(State state)
{
   if (mState == state)
   {
      return;
   }

   mState = state;
   switch (mState)
   {
      case NORMAL:
         break;
      case HOVER:
         break;
      case PRESS:
         break;
   }
}

}; // namespace Engine

}; // namespace CubeWorld

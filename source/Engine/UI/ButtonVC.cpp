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
   if (rec.GetState() == Engine::UIGestureRecognizer::Ending) {
      mClickCallback();
   } else {
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
         mIdleCallback(this);
         break;
      case HOVER:
         mHoverCallback(this);
         break;
      case PRESS:
         mPressCallback(this);
         break;
   }
}

}; // namespace Engine

}; // namespace CubeWorld

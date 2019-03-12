//
//  UITapGestureRecognizer.cpp
//  Engine
//
//  Created by Elliot Fiske on 10/18/18.
//

#include "UITapGestureRecognizer.h"

#include <Engine/UI/UIElement.h>

namespace CubeWorld
{
   
namespace Engine
{
   
UITapGestureRecognizer::UITapGestureRecognizer(UIElement* element, GestureCallback callback)
   : UIGestureRecognizer(element, callback)
   , mbStartedInsideMe(false)
{
}

void UITapGestureRecognizer::MouseMove(const MouseMoveEvent& evt)
{
   if (mpElement->ContainsPoint(evt.x, evt.y)) {
      if (mbStartedInsideMe && mState == Possible) {  // Mouse clicked button, dragged off then dragged back on
         mState = Happening;
         mCallback(*this);
      }
   }
   else 
   {
      if (mState == Happening) {
         mState = Possible;
         mCallback(*this);
      }
   }
}

void UITapGestureRecognizer::MouseDown(const MouseDownEvent& evt)
{
   if (mpElement->ContainsPoint(evt.x, evt.y)) {
      mState = Happening;
      mbStartedInsideMe = true;
      mCallback(*this);
   }
}

void UITapGestureRecognizer::MouseUp(const MouseUpEvent& evt)
{
   if (mState == Happening) {
      mState = Ending;
      mCallback(*this);
      mState = Possible;
   }
   
   mbStartedInsideMe = false;
}
   
} // namespace Engine

} // namespace CubeWorld

//
//  UIClickGestureRecognizer.cpp
//  Engine
//
//  Created by Elliot Fiske on 10/18/18.
//

#include "UIClickGestureRecognizer.h"
#include <RGBBinding/ObservableBasicOperations.h>

#include <Engine/UI/UIElement.h>

namespace CubeWorld
{
   
namespace Engine
{
   
UIClickGestureRecognizer::UIClickGestureRecognizer(UIElement* element)
   : UIGestureRecognizer(element)
   , mbStartedInsideMe(false)
{
}

bool UIClickGestureRecognizer::MouseMove(const MouseMoveEvent& evt)
{
   if (mpElement->ContainsPoint(evt.x, evt.y)) {
      if (mbStartedInsideMe && mState == Possible) {  // Mouse clicked inside element, dragged off then dragged back on
         ChangeStateAndBroadcastMessage(Happening, evt.x, evt.y);
      }
   }
   else
   {
      if (mState == Happening) {
         ChangeStateAndBroadcastMessage(Possible, evt.x, evt.y);
      }
   }

   return false;
}

bool UIClickGestureRecognizer::MouseDown(const MouseDownEvent& evt)
{
   ChangeStateAndBroadcastMessage(Happening, evt.x, evt.y);
   mbStartedInsideMe = true;
   return true;
}

void UIClickGestureRecognizer::MouseUp(const MouseUpEvent& evt)
{
   if (mState == Happening && mbStartedInsideMe && mpElement->ContainsPoint(evt.x, evt.y)) {
      ChangeStateAndBroadcastMessage(Ending, evt.x, evt.y);
      ChangeStateAndBroadcastMessage(Possible, evt.x, evt.y);
   }
   
   mbStartedInsideMe = false;
}

Observables::Observable<UIGestureRecognizer::Message_GestureState> &UIClickGestureRecognizer::OnClick()
{
   return mStateChangedObservable.MessageProducer() >>
      Observables::Filter<Message_GestureState>([](Message_GestureState m) {
         return m.state == Ending;
      });
}

} // namespace Engine

} // namespace CubeWorld

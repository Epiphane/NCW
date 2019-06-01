//
//  UIGestureRecognizer.cpp
//  Engine
//
//  Created by Elliot Fiske on 10/18/18.
//

#include "UIGestureRecognizer.h"

namespace CubeWorld
{
   
namespace Engine
{

UIGestureRecognizer::UIGestureRecognizer(UIElement* element)
      : mpElement(element)
      , mState(Possible)
{
}

Observables::Observable<UIGestureRecognizer::Message_GestureState> &UIGestureRecognizer::OnStateChanged()
{
   return mStateObservable.OnChanged();
}

void UIGestureRecognizer::ChangeStateAndBroadcastMessage(UIGestureRecognizer::State newState, double mouseX, double mouseY)
{
   mState = newState;
   mStateObservable.SendMessage({newState, mouseX, mouseY});
}

} // namespace Engine
   
} // namespace CubeWorld

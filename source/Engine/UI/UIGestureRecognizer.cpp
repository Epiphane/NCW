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

UIGestureRecognizer::UIGestureRecognizer(UIElement* element, GestureCallback callback)
   : mState(Possible)
   , mpElement(element)
   , mCallback(callback)
{
}

UIGestureRecognizer::~UIGestureRecognizer()
{
}

} // namespace Engine
   
} // namespace CubeWorld

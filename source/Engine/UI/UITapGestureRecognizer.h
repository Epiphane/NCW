//
//  UITapGestureRecognizer.h
//
//  Created by Elliot Fiske on 10/18/18.
//

#pragma once

#include "UIGestureRecognizer.h"

namespace CubeWorld
{
    
namespace Engine
{
    
class UITapGestureRecognizer : public UIGestureRecognizer {
public:
   UITapGestureRecognizer(UIElement* element, GestureCallback callback);
   
   virtual void MouseMove(const MouseMoveEvent& evt);
   virtual void MouseDown(const MouseDownEvent& evt);
   virtual void MouseUp(const MouseUpEvent& evt);
   
protected:
   bool mbStartedInsideMe; ///< oh my
   
};

} // namespace Engine
   
} // namespace CubeWorld

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
   UITapGestureRecognizer(UIElement* element);

   virtual bool MouseMove(const MouseMoveEvent& evt);
   virtual bool MouseDown(const MouseDownEvent& evt);
   virtual void MouseUp(const MouseUpEvent& evt);

   // This lets you observe JUST when the there has been a successful
   //    mouse click on this element.
   Observables::Observable<Message_GestureState>& OnTap();

protected:
   bool mbStartedInsideMe; ///< oh my
};

} // namespace Engine
   
} // namespace CubeWorld

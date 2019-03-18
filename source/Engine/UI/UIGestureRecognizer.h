//
//  UIGestureRecognizer.hpp
//
//  Created by Elliot Fiske on 10/18/18.
//

#pragma once

#include <functional>

#include "../Event/InputEvent.h"

namespace CubeWorld
{
   
namespace Engine
{
   
   class UIGestureRecognizerDelegate; ///< Forward declare
   class UIGestureRecognizer;
   class UIElement;
   
   typedef std::function<void(const UIGestureRecognizer&)> GestureCallback;
   
   class UIGestureRecognizer {
   public:
      UIGestureRecognizer(UIElement* element, GestureCallback callback);
      virtual ~UIGestureRecognizer();
      
      //
      // What part of the gesture's lifecycle is this recognizer on?
      //
      // Example: On mouse down, a PanGestureRecognizer will become 'Possible'. When the mouse moves,
      //            we know a pan is happening so the recognizer is now 'Happening'. On mouse up,
      //            pan is set to 'Ending'.
      //
      enum State {
         Possible,
         Happening,
         Ending,
         Cancelled    ///< Usually triggered externally (i.e. if a Scrollview becomes hidden while scrolling)
      };
      
      State GetState() const { return mState; }
      
      virtual void MouseMove(const MouseMoveEvent& evt) = 0;
      virtual void MouseDown(const MouseDownEvent& evt) = 0;
      virtual void MouseUp(const MouseUpEvent& evt) = 0;
      
   protected:
      State mState;
      
      // Which UIElement is this recognizer looking for clicks on?
      UIElement* mpElement;
      
      UIGestureRecognizerDelegate* mpDelegate;
      
      // The gesture recognizer calls this whenever relevant changes happen.
      //    i.e. when a UITapGestureRecognizer receives a mouse down, or when 
      //    a UIPanGestureRecognizer receives a mouse drag.
      GestureCallback mCallback;
   };
   
   //
   // Lets you fine-tune the behavior of a gesture recognizer.
   //
   class UIGestureRecognizerDelegate {
      
   };
   
} // namespace Engine
   
} // namespace CubeWorld

//
//  UIGestureRecognizer.hpp
//
//  Created by Elliot Fiske on 10/18/18.
//

#pragma once

#include <functional>

#include <RGBBinding/Observable.h>

#include "../Event/InputEvent.h"

namespace CubeWorld
{
   
namespace Engine
{
   
   using Observables::ObservableInternal;
   
   class UIGestureRecognizer; // Forward declare
   class UIElement;

   class UIGestureRecognizer {
   public:
      UIGestureRecognizer(UIElement* element);

      //
      // What part of the gesture's lifecycle is this recognizer on?
      //
      // Example: On mouse down, a PanGestureRecognizer will become 'Possible'. When the mouse moves,
      //            we know a pan is happening so the recognizer is now 'Happening'. On mouse up,
      //            pan is set to 'Ending'.
      //
      enum State {
         Possible,
         Starting,
         Happening,
         Ending,
         Cancelled    ///< Usually triggered externally (i.e. if a Scrollview becomes hidden while scrolling)
      };

      // Messages emitted that describe the gesture recognizer's state
      struct Message_GestureState {
         State state;
         double gestureX;
         double gestureY;
      };

      // If this returns true, this recognizer will Capture the current mouse click.
      virtual bool MouseMove(const MouseMoveEvent& evt) = 0;

      // If this returns true, this recognizer will Capture the current mouse click.
      virtual bool MouseDown(const MouseDownEvent& evt) = 0;

      // You can't capture a mouse click as it's ending.
      virtual void MouseUp(const MouseUpEvent& evt) = 0;

      // Observable that sends a message whenever the state of this recognizer changes
      Observables::Observable<Message_GestureState>& OnStateChanged();
      
   protected:
      // Helper function to change mState and send a message about it
      void ChangeStateAndBroadcastMessage(State newState, double mouseX, double mouseY);

      ObservableInternal<Message_GestureState> mStateChangedObservable;

      // Which UIElement is this recognizer looking for clicks on?
      UIElement* mpElement;

      State mState;
   };

} // namespace Engine
   
} // namespace CubeWorld

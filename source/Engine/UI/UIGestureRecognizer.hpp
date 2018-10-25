//
//  UIGestureRecognizer.hpp
//
//  Created by Elliot Fiske on 10/18/18.
//

#pragma once

namespace CubeWorld
{
    
namespace Engine
{

/**
 * Lets you fine-tune the behavior of a gesture recognizer.
 */
class UIGestureRecognizerDelegate {
    
};
    
class UITouch {
public:
    bool GetConsumed() { return mbConsumed; }
    
private:
    bool mbConsumed;
};
    
class UIGestureRecognizer {
public:
    /**
     * What part of the gesture's lifecycle is this recognizer on?
     *
     * Example: On mouse down, a PanGestureRecognizer will become 'Possible'. When the mouse moves,
     *              we know a pan is happening so the recognizer is now 'Happening'. On mouse up,
     *              pan is set to 'Ending'.
     */
    enum State {
        Possible,
        Happening,
        Ending,
        Cancelled    ///< Usually triggered externally (i.e. if a Scrollview becomes hidden while scrolling)
    };
    
private:
    State mState;
    
    UIGestureRecognizerDelegate mDelegate;
};
    

}
    
}

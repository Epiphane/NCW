//
// UISystem - Provides the ability to render game UI components
//
// By Thomas Steinke
//

#pragma once

#include <vector>

#include "../Core/Input.h"
#include "../Event/EventManager.h"
#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class UIRoot : public EventManager, public UIElement
{

public:
    UIRoot(Input* input);
    ~UIRoot();

    //
    // Set the manager responsible for providing input to this UI.
    //
    void SetInput(Input* input) { mInput = input; }

    //
    // Get the associated input manager.
    //
    Input* GetInput() const { return mInput; }

private:
    // Input manager.
    Input* mInput;

    // The element, if any, that has captured the current click-and-drag of the mouse.
    UIElement* mActivelyCapturingElement;

    // Tracks whether something has rebalanced in the last frame.
    bool mDirty;
};

}; // namespace Engine

}; // namespace CubeWorld

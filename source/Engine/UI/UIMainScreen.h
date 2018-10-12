//
// UIMainScreen – contains the standard game UI. Pause screen, overlays, etc.
//
// By Thomas Steinke
//

#pragma once

#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{

class UIMainScreen : public UIRoot {
public:
   UIMainScreen(const Bounded& bounds);
};

}; // namespace Engine

}; // namespace CubeWorld

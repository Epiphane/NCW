//
// UIMainScreen – contains the standard game UI. Pause screen, overlays, etc.
//
// By Thomas Steinke
//

#pragma once

#include <Engine/UI/UIRoot.h>
#include <Engine/UI/UIRectFilled.h>

namespace CubeWorld
{

namespace Engine
{
   class UIMainScreen : public UIRoot {
   public:
      UIMainScreen(Window* pWindow);
   };
   
}; // namespace Engine

}; // namespace CubeWorld

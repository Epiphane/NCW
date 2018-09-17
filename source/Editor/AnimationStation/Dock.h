// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>

#include "../UI/SubWindow.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class Dock : public SubWindow {
public:
   Dock(
      Bounded& parent,
      const Options& options,
      MainState* state
   );

private:
   MainState* mState;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

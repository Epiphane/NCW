//
// UIMainScreen – contains the standard game UI. Pause screen, overlays, etc.
//
// By Thomas Steinke
//

#pragma once

#include <Engine/UI/UIRootDep.h>

namespace CubeWorld
{

namespace Game
{

class UIMainScreen : public Engine::UIRootDep {
public:
   UIMainScreen(Engine::Input* input);
};

}; // namespace Game

}; // namespace CubeWorld

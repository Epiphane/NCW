// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/State.h>

namespace CubeWorld
{

namespace Game
{

class StupidState : public Engine::State {
public:
   StupidState();
   ~StupidState();

   void Start() override;
};

}; // namespace Game

}; // namespace CubeWorld

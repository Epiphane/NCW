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

private:
   std::unique_ptr<Engine::Graphics::Camera> mCam;
};

}; // namespace Game

}; // namespace CubeWorld

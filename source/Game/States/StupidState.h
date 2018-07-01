// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/Camera.h>

namespace CubeWorld
{

namespace Game
{

class StupidState : public Engine::State {
public:
   StupidState(Engine::Window* window);
   ~StupidState();

   void Start() override;

private:
   Engine::Graphics::CameraHandle mCamera;

   Engine::Window* mWindow;
};

}; // namespace Game

}; // namespace CubeWorld

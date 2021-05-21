// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>
#include <WorldGenerator/World/World.h>

namespace CubeWorld
{

namespace Game
{

class StupidState : public Engine::State, public Engine::Receiver<StupidState> {
public:
   StupidState(Engine::Window& window);
   ~StupidState();

   void Initialize() override;

   bool BuildFloorCollision(int32_t size);

private:
   std::unique_ptr<Engine::Input::KeyCallbackLink> mDebugCallback;

   Engine::Graphics::CameraHandle mCamera;

   World mWorld;
   Engine::Window& mWindow;
   std::vector<int32_t> heights;
};

}; // namespace Game

}; // namespace CubeWorld

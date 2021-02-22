// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>
#include "../World/World.h"

namespace CubeWorld
{

class MainState : public Engine::State, public Engine::Receiver<MainState> {
public:
   MainState(Engine::Window& window);
   ~MainState();

   void Initialize() override;

private:
   std::unique_ptr<Engine::Input::KeyCallbackLink> mDebugCallback;

   Engine::Graphics::CameraHandle mCamera;

   Engine::Window& mWindow;

   World mWorld;
   std::vector<int32_t> heights;
};

}; // namespace CubeWorld
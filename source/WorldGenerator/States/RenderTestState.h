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

class RenderTestState : public Engine::State, public Engine::Receiver<RenderTestState> {
public:
    RenderTestState(Engine::Window& window);
   ~RenderTestState();

   void Initialize() override;
   void Update(TIMEDELTA dt) override;

private:
   std::unique_ptr<Engine::Input::KeyCallbackLink> mDebugCallback;

   Engine::Graphics::CameraHandle mCamera;

   Engine::Window& mWindow;
};

}; // namespace CubeWorld

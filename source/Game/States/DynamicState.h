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

class DynamicState : public Engine::State, public Engine::Receiver<DynamicState> {
public:
    DynamicState(Engine::Window& window);

   void Initialize() override;
   void Pause() override;
   void Unpause() override;
   void Update(TIMEDELTA dt) override;

private:
   std::unique_ptr<Engine::Input::KeyCallbackLink> mDebugCallback;

   Engine::Graphics::CameraHandle mCamera;

   World mWorld;
   Engine::Window& mWindow;
   std::vector<int32_t> heights;
};

}; // namespace Game

}; // namespace CubeWorld

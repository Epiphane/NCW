// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Game/Event/NamedEvent.h>

namespace CubeWorld
{

namespace Game
{

class AnimationStation : public Engine::State, public Engine::Receiver<AnimationStation> {
public:
   AnimationStation(Engine::Window* window);
   ~AnimationStation();

   void Start() override;

   void Receive(const NamedEvent& namedEvent);

private:
   Engine::Graphics::CameraHandle mCamera;

   Engine::Window* mWindow;
   std::vector<int32_t> heights;
};

}; // namespace Game

}; // namespace CubeWorld

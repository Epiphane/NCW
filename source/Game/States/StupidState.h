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

class StupidState : public Engine::State, public Engine::Receiver<StupidState> {
public:
   StupidState(Engine::Window* window);
   ~StupidState();

   void Start() override;

   void Receive(const NamedEvent& namedEvent);
   
   bool BuildFloorCollision(int32_t size, const std::vector<int32_t>& heights);

private:
   Engine::Graphics::CameraHandle mCamera;

   Engine::Window* mWindow;
   int mSkip = 0;
   std::vector<int32_t> heights;
};

}; // namespace Game

}; // namespace CubeWorld

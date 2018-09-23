// By Thomas Steinke

#pragma once

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class MainState : public Engine::State, public Engine::Receiver<MainState> {
public:
   MainState(Engine::Window* window, Bounded& parent);
   ~MainState();

   void Initialize() override;
   void SetParent(Engine::EventManager* other) { mEvents.SetParent(other); }

private:
   Engine::Graphics::CameraHandle mCamera;

   Engine::Window* mWindow;
   Bounded& mParent;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

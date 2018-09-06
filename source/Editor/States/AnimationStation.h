// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>

namespace CubeWorld
{

namespace Editor
{

class AnimationStation : public Engine::State, public Engine::Receiver<AnimationStation> {
public:
   AnimationStation(Engine::Window* window);
   ~AnimationStation();

   void Start() override;

   void Receive(const NamedEvent& namedEvent);
   void Receive(const MouseDragEvent& evt);
   void Receive(const MouseDownEvent& evt);
   void Receive(const MouseUpEvent& evt);
   void Receive(const MouseClickEvent& evt);

private:
   Engine::Graphics::CameraHandle mCamera;

   Engine::Window* mWindow;
   std::vector<int32_t> heights;
};

}; // namespace Editor

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>

#include <Engine/Core/Input.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Event/EventManager.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Event/Receiver.h>
#include <Engine/System/System.h>

// Arm cameras are defined separately, since they aren't strictly tied to this system.
#include "../Components/ArmCamera.h"

namespace CubeWorld
{

struct MouseControlledCamera : public Engine::Component<MouseControlledCamera> {
   MouseControlledCamera(double pitchSensitivity = 0.007, double yawSensitivity = 0.007)
      : sensitivity{pitchSensitivity, yawSensitivity}
   {};
   
   double sensitivity[2];
};

struct MouseDragCamera : public Engine::Component<MouseDragCamera> {
   MouseDragCamera(uint32_t button, double pitchSensitivity = 0.007, double yawSensitivity = 0.007)
      : button(button)
      , sensitivity{pitchSensitivity, yawSensitivity}
   {};

   uint32_t button;
   double sensitivity[2];
   bool engaged;
};

struct KeyControlledCamera : public Engine::Component<KeyControlledCamera> {
   KeyControlledCamera(
      uint32_t up = GLFW_KEY_W,
      uint32_t down = GLFW_KEY_S,
      uint32_t left = GLFW_KEY_A,
      uint32_t right = GLFW_KEY_D,
      double pitchSpeed = 2.0,
      double yawSpeed = 2.0
   ) 
      : keys{up, down, left, right}
      , sensitivity{pitchSpeed, yawSpeed}
   {};
   
   uint32_t keys[4];
   double sensitivity[2];
};

class CameraSystem : public Engine::System<CameraSystem>, public Engine::Receiver<CameraSystem> {
public:
   CameraSystem(Engine::Input* input) : mInput(input) {}
   ~CameraSystem() {}
   
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
   
public:
   void Receive(const Engine::ComponentAddedEvent<MouseDragCamera>& evt);
   void Receive(const MouseDownEvent& evt);
   void Receive(const MouseUpEvent& evt);

private:
   Engine::Input* mInput;

   std::vector<Engine::ComponentHandle<MouseDragCamera>> mDraggables;
};

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <memory>
#include <vector>

#include <Engine/Core/Input.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Event/EventManager.h>
#include <Engine/System/System.h>

// Arm cameras are defined separately, since they aren't strictly tied to this system.
#include "../Components/ArmCamera.h"

namespace CubeWorld
{

namespace Game
{

struct MouseControlledCamera : public Engine::Component<MouseControlledCamera> {
   MouseControlledCamera(double pitchSensitivity = 0.007, double yawSensitivity = 0.007)
      : sensitivity{pitchSensitivity, yawSensitivity}
   {};
   
   double sensitivity[2];
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

class CameraSystem : public Engine::System<CameraSystem> {
public:
   CameraSystem(Engine::Input::InputManager* input) : mInput(input) {}
   ~CameraSystem() {}
   
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
   
private:
   Engine::Input::InputManager* mInput;
};

}; // namespace Game

}; // namespace CubeWorld

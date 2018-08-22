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
#include <Game/Components/ArmCamera.h>

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

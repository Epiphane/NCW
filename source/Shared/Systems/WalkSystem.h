// By Thomas Steinke

#pragma once

#include <optional>
#include <Engine/Core/Input.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

struct WalkDirector : public Engine::Component<WalkDirector> {
   WalkDirector(Engine::ComponentHandle<Engine::Transform> director, bool anchor)
      : director(director)
      , anchor(anchor)
   {};

   Engine::ComponentHandle<Engine::Transform> director;
   bool anchor;
};

struct WalkSpeed : public Engine::Component<WalkSpeed> {
   WalkSpeed(float runSpeed, float walkSpeed, float accel)
      : walkSpeed(walkSpeed)
      , runSpeed(runSpeed)
      , accel(accel)
      , currentSpeed(0)
      , walking(false)
      , running(false)
   {};

   float walkSpeed;
   float runSpeed;
   float accel;
   float currentSpeed;
   std::optional<float> tempMaxSpeed;
   bool walking;
   bool running;
};

class WalkSystem : public Engine::System<WalkSystem> {
public:
   WalkSystem(Engine::Input* input) : mInput(input) {}
   ~WalkSystem() {}

   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

private:
   Engine::Input* mInput;
};

}; // namespace CubeWorld

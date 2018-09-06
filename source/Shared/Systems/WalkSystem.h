// By Thomas Steinke

#pragma once

#include <Engine/Core/Input.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

namespace Game
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
      WalkSpeed(double runSpeed, double walkSpeed, double accel)
         : walkSpeed(walkSpeed)
         , runSpeed(runSpeed)
         , accel(accel)
         , currentSpeed(0)
         , walking(false)
         , running(false)
      {};
      
      bool walking;
      bool running;
      double walkSpeed;
      double runSpeed;
      double accel;
      double currentSpeed;
   };
   
   class WalkSystem : public Engine::System<WalkSystem> {
   public:
      WalkSystem(Engine::Input::InputManager* input) : mInput(input) {}
      ~WalkSystem() {}
      
      void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
      
   private:
      Engine::Input::InputManager* mInput;
   };
}; // namespace Game

}; // namespace CubeWorld

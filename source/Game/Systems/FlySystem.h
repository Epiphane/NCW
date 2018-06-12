// By Thomas Steinke

#pragma once

#include <Engine/Core/Input.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

namespace Game
{
   struct FlySpeed : public Engine::Component<FlySpeed> {
      FlySpeed(double speed) : speed(speed)
      {};
      
      double speed;
   };
   
   class FlySystem : public Engine::System<FlySystem> {
   public:
      FlySystem(Engine::Input::InputManager* input) : mInput(input) {}
      ~FlySystem() {}
      
      void Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt) override;
      
   private:
      Engine::Input::InputManager* mInput;
   };
}; // namespace Game

}; // namespace CubeWorld

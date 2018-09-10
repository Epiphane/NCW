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
      FlySystem(Engine::Input* input) : mInput(input) {}
      ~FlySystem() {}
      
      void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
      
   private:
      Engine::Input* mInput;
   };
}; // namespace Game

}; // namespace CubeWorld

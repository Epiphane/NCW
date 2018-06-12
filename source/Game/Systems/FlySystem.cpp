// By Thomas Steinke

#include "FlySystem.h"

namespace CubeWorld
{

namespace Game
{
   
   void FlySystem::Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt)
   {
      bool isW = mInput->IsKeyDown(GLFW_KEY_W);
      bool isA = mInput->IsKeyDown(GLFW_KEY_A);
      bool isS = mInput->IsKeyDown(GLFW_KEY_S);
      bool isD = mInput->IsKeyDown(GLFW_KEY_D);
      
      // TODO why can't I just put a lambda in here?
      std::function<void(Engine::Entity, Engine::Transform&, FlySpeed&)> fn = [&](Engine::Entity /*entity*/, Engine::Transform& transform, FlySpeed& speed) {
      };
      entities.Each<Engine::Transform, FlySpeed>(fn);
   }
   
}; // namespace Game

}; // namespace CubeWorld

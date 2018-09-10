// By Thomas Steinke

#include <Engine/Logger/Logger.h>

#include "FollowerSystem.h"

namespace CubeWorld
{

namespace Game
{
   
   void FollowerSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
   {
      entities.Each<Engine::Transform, Follower>([&](Engine::Entity, Engine::Transform& transform, Follower& follower) {
         glm::vec3 target = follower.target->GetAbsolutePosition();
         glm::vec3 current = transform.GetAbsolutePosition();

         // TODO lol
         assert(!transform.GetParent());

         float move = float(follower.elasticity * dt);
         transform.SetLocalPosition(move * target + (1 - move) * current);
      });
   }
   
}; // namespace Game

}; // namespace CubeWorld

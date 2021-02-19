// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "FollowerSystem.h"

namespace CubeWorld
{

void FollowerSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   entities.Each<Engine::Transform, Follower>([&](Engine::Entity, Engine::Transform& transform, Follower& follower) {
      glm::vec3 target = follower.target->GetAbsolutePosition();
      glm::vec3 current = transform.GetAbsolutePosition();

      // TODO lol
      assert(!transform.GetParent());


      if (glm::length(follower.elasticity) <= glm::epsilon<float>())
      {
          transform.SetLocalPosition(target);
      }
      else
      {
          glm::vec3 move = float(dt) * follower.elasticity;
          glm::vec3 interp{
             move.x * target.x + (1 - move.x) * current.x,
             move.y * target.y + (1 - move.y) * current.y,
             move.z * target.z + (1 - move.z) * current.z,
          };

          transform.SetLocalPosition(interp);
      }
   });
}

}; // namespace CubeWorld

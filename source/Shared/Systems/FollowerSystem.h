// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>

namespace CubeWorld
{

struct Follower : public Engine::Component<Follower> {
   Follower(Engine::ComponentHandle<Engine::Transform> target, float elasticity = 10.0f)
      : Follower(target, glm::vec3{elasticity})
   {};
   Follower(Engine::ComponentHandle<Engine::Transform> target, const glm::vec3& elasticity)
      : target(target)
      , elasticity(elasticity)
   {};
   
   Engine::ComponentHandle<Engine::Transform> target;
   glm::vec3 elasticity;
};

class FollowerSystem : public Engine::System<FollowerSystem> {
public:
   FollowerSystem() {}
   ~FollowerSystem() {}
   
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace CubeWorld

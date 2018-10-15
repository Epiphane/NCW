// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>

namespace CubeWorld
{

struct Follower : public Engine::Component<Follower> {
   Follower(Engine::ComponentHandle<Engine::Transform> target, float elasticity = 10.0f)
      : target(target)
      , elasticity(elasticity)
   {};
   
   Engine::ComponentHandle<Engine::Transform> target;
   float elasticity;
};

class FollowerSystem : public Engine::System<FollowerSystem> {
public:
   FollowerSystem() {}
   ~FollowerSystem() {}
   
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace CubeWorld

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
   Follower(const std::unordered_map<uint64_t, Engine::Entity>& entities, const BindingProperty& data);
   
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

namespace meta
{

using CubeWorld::Follower;

template<>
inline auto registerMembers<Follower>()
{
    return members(
        member("elasticity", &Follower::elasticity)
    );
}

}; // namespace meta

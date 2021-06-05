// By Thomas Steinke

#pragma once

#include <optional>

#include <RGBBinding/BindingPropertyMeta.h>
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
   WalkDirector(const std::unordered_map<uint64_t, Engine::Entity>& entities, const BindingProperty& data);

   Engine::ComponentHandle<Engine::Transform> director;
   bool anchor;
};

struct WalkSpeed : public Engine::Component<WalkSpeed> {
   WalkSpeed(float runSpeed, float walkSpeed, float accel)
      : walkSpeed(walkSpeed)
      , runSpeed(runSpeed)
      , accel(accel)
   {};
   WalkSpeed(const BindingProperty& data);

   float walkSpeed;
   float runSpeed;
   float accel;
   float currentSpeed = 0;
   std::optional<float> tempMaxSpeed;
   bool walking = false;
   bool running = false;
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

namespace meta
{

using CubeWorld::WalkDirector;
using CubeWorld::WalkSpeed;

template<>
inline auto registerMembers<WalkDirector>()
{
    return members(
        member("anchor", &WalkDirector::anchor)
    );
}

template<>
inline auto registerMembers<WalkSpeed>()
{
    return members(
        member("walk_speed", &WalkSpeed::walkSpeed),
        member("run_speed", &WalkSpeed::runSpeed),
        member("accel", &WalkSpeed::accel)
    );
}

}; // namespace meta

// By Thomas Steinke

#pragma once

#include <Shared/Systems/AnimationSystem.h>

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

struct AnimationSystemController : public Engine::Component<AnimationSystemController>
{
   //
   // Editing switches. Pause all animations (but do not disable) and disable transitions.
   //
   double speed = 1.0;
   bool paused = true;
   TIMEDELTA nextTick = 0;
};

//
// AnimationStation::AnimationSystem extends Game::AnimationSystem and allows for control by
// an entity containing the AnimationSystemController component.
//
class AnimationSystem : public Engine::System<AnimationSystem>, private BaseAnimationSystem
{
public:
   AnimationSystem() {};
   ~AnimationSystem() {}

   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

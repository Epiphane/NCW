// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>
#include "../DebugHelper.h"
#include "../Components/AnimationController.h"

namespace CubeWorld
{

class AnimationSystem : public Engine::System<AnimationSystem> {
public:
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events);
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);

private:
   void UpdateEmitters(
      const Engine::Transform& transform,
      const AnimationController& controller,
      const AnimationController::State& state,
      bool updateAllTransforms
   ) const;
};

}; // namespace CubeWorld

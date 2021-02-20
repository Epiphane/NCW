// By Thomas Steinke

#pragma once

#include <string>
#include <unordered_set>
#include <Engine/Core/Input.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/System/System.h>

#include "WalkSystem.h"

namespace CubeWorld
{

class WalkAnimationSystem : public Engine::System<WalkAnimationSystem> {
public:
   WalkAnimationSystem() {}
   ~WalkAnimationSystem() {}

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

private:
   static void BlendState(
      const std::vector<bool>& mask,
      AnimationController& anim,
      const AnimationController::State& state,
      float blend
   );
};

}; // namespace CubeWorld

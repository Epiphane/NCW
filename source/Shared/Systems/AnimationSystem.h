// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>
#include "../DebugHelper.h"
#include "../Components/AnimationController.h"

namespace CubeWorld
{

class AnimationSystem : public Engine::System<AnimationSystem>
{
public:
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events);
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);
};

class AnimationApplicator : public Engine::System<AnimationSystem>
{
public:
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);

private:
   void UpdateEmitters(
      Engine::EntityManager& entities,
      AnimationController& controller,
      const Engine::Transform& transform,
      AnimationController::State& state,
      bool updateAllTransforms
   ) const;
};

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <unordered_map>
#include <vector>

#include <Engine/System/System.h>
#include "../Components/AnimationController.h"
#include "../Components/VoxModel.h"

namespace CubeWorld
{

class BaseAnimationSystem {
public:
   BaseAnimationSystem() : mAnimate(true), mTransitions(true) {};
   ~BaseAnimationSystem() {}
   
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);

protected:
   // Whether animation and transitions are enabled. Used for editing.
   bool mAnimate;
   bool mTransitions;
};

class AnimationSystem : public Engine::System<AnimationSystem>, public BaseAnimationSystem {
public:
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override
   {
      BaseAnimationSystem::Update(entities, events, dt);
   }
};

}; // namespace CubeWorld

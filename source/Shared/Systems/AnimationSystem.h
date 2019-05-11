// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>
#include "../Components/AnimationController.h"

namespace CubeWorld
{

class AnimationSystem : public Engine::System<AnimationSystem> {
public:
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);
};

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>
#include "../DebugHelper.h"
#include "../Components/AnimationController.h"

namespace CubeWorld
{

class AnimationEventSystem : public Engine::System<AnimationEventSystem> {
public:
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events);
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);
};

}; // namespace CubeWorld

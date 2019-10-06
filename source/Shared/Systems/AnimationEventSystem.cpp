// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBLogger/Logger.h>
#include <Engine/Core/Config.h>

#include "../Components/VoxModel.h"
#include "AnimationEventSystem.h"

namespace CubeWorld
{

void AnimationEventSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{}

void AnimationEventSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   // First, update skeletons.
   entities.Each<AnimationController>([&](Engine::Entity, AnimationController& controller) {
      // Check for an un-loaded skeleton
      if (controller.skeletons.empty())
      {
         return;
      }
   });
}

}; // namespace CubeWorld

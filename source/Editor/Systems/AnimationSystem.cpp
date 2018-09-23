// By Thomas Steinke

#include "AnimationSystem.h"

namespace CubeWorld
{

namespace Editor
{

void AnimationSystem::Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt)
{
   auto controllers = entities.EntitiesWithComponents<AnimationSystemController>();
   if (auto entity = controllers.begin(); entity != controllers.end())
   {
      auto controller = (*entity).Get<AnimationSystemController>();

      if (controller->paused)
      {
         dt = controller->nextTick;
         controller->nextTick = 0;
      }
      dt *= controller->speed;

      mTransitions = controller->transitions;
   }

   Game::BaseAnimationSystem::Update(entities, events, dt);
}

}; // namespace Game

}; // namespace CubeWorld

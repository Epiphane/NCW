// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "CombatSystem.h"

namespace CubeWorld
{

void CombatSystem::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
   events.Subscribe<StrikeEvent>(*this);
}

void CombatSystem::Update(Engine::EntityManager&, Engine::EventManager&, TIMEDELTA)
{
}

void CombatSystem::Receive(const StrikeEvent& evt)
{
   Engine::ComponentHandle<UnitComponent> source = evt.source.Get<UnitComponent>();
   Engine::ComponentHandle<UnitComponent> target = evt.target.Get<UnitComponent>();

   if (!source || !target)
   {
      LOG_DEBUG(
         "Entity %1 strikes entity %2, but one of them is missing a UnitComponent",
         evt.source.GetID().index(),
         evt.target.GetID().index()
      );
   }

   if (target->health > 0)
   {
      target->health--;
   }

   if (target->health == 0)
   {
      target->dead = true;

      if (evt.target.Has<AnimationController>())
      {
         evt.target.Get<AnimationController>()->SetBoolParameter("dead", true);
      }
   }
}

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>
#include "AnimationEventSystem.h"

namespace CubeWorld
{

struct StrikeEvent : public Engine::Event<StrikeEvent>
{
   StrikeEvent(Engine::Entity src, Engine::Entity trg)
      : source(src)
      , target(trg)
   {}

   Engine::Entity source;
   Engine::Entity target;
};

struct UnitComponent : public Engine::Component<UnitComponent> {
   UnitComponent(int health) : UnitComponent((uint32_t)health) {};
   UnitComponent(uint32_t health)
      : health(health)
      , maxHealth(health)
   {};
   
   uint32_t health;
   uint32_t maxHealth;
   bool dead = false;
};

class CombatSystem : public Engine::System<CombatSystem>, public Engine::Receiver<CombatSystem> {
public:
   CombatSystem() {}
   ~CombatSystem() {}
   
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void Receive(const StrikeEvent& evt);
};

}; // namespace CubeWorld

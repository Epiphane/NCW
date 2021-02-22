// By Thomas Steinke

#pragma once

#include <vector>
#include <Engine/Event/Event.h>
#include <Engine/System/System.h>
#include "../DebugHelper.h"
#include "../Components/AnimationController.h"
#include "Simple3DRenderSystem.h"
#include "CombatSystem.h"

namespace CubeWorld
{

//
// AnimationEventSystem tracks and manages interactions between animations and the world,
// e.g. weapons hitting enemies.
//
class AnimationEventSystem : public Engine::System<AnimationEventSystem> {
public:
   struct StaleObject
   {
      std::unique_ptr<btCollisionShape> shape;
      std::unique_ptr<btGhostObject> ghost;
   };

public:
   AnimationEventSystem(BulletPhysics::System* physics)
      : mPhysics(physics)
   {
      assert(physics != nullptr);
   }

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events);
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);

private:
   BulletPhysics::System* mPhysics;

   std::vector<StaleObject> mStaleObjects;
};

}; // namespace CubeWorld

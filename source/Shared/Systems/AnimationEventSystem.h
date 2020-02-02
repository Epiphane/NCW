// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>
#include "../DebugHelper.h"
#include "../Components/AnimationController.h"
#include "Simple3DRenderSystem.h"
#include "BulletPhysicsSystem.h"

namespace CubeWorld
{

struct AnimationEventDebugger : public Engine::Component<AnimationEventDebugger>
{
   AnimationEventDebugger(Engine::ComponentHandle<Simple3DRender> output) : output(output) {};

   Engine::ComponentHandle<Simple3DRender> output;
};

class AnimationEventSystem : public Engine::System<AnimationEventSystem> {
public:
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events);
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);
};

//
// AnimationEventDebugSystem is useful for visually representing animation information, such as events.
//
class AnimationEventDebugSystem : public Engine::System<AnimationEventDebugSystem> {
public:
   AnimationEventDebugSystem(
      BulletPhysics::System* physics = nullptr,
      bool active = true
   )
      : mPhysics(physics)
      , mActive(active)
   {}

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void SetActive(bool active) { mActive = active; }
   bool IsActive() { return mActive; }

private:
   void Extend(
      std::vector<GLfloat>& points,
      std::vector<GLfloat>& colors,
      glm::mat4 matrix,
      glm::vec3 offset,
      glm::vec3 size
   );

   BulletPhysics::System* mPhysics;
   bool mActive;
};

}; // namespace CubeWorld

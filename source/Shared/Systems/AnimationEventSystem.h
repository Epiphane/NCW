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

//
// AnimationEventDebugSystem is useful for visually representing animation information, such as events.
//
class AnimationEventDebugSystem : public Engine::System<AnimationEventDebugSystem> {
public:
   AnimationEventDebugSystem(bool active = true, Engine::Graphics::Camera* camera = nullptr) : mActive(active), mCamera(camera) {}

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void SetActive(bool active) { mActive = active; }
   bool IsActive() { return mActive; }

   void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }

private:
   bool mActive;

private:
   Engine::Graphics::Camera* mCamera;

   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace CubeWorld

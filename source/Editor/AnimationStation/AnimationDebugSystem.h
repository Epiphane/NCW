// By Thomas Steinke

#pragma once

#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/Program.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

//
// AnimationDebugSystem is useful for visually representing animation information, such as events.
//
class AnimationDebugSystem : public Engine::System<AnimationDebugSystem> {
public:
   AnimationDebugSystem(bool active = true, Engine::Graphics::Camera* camera = nullptr) : mActive(active), mCamera(camera) {}

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

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

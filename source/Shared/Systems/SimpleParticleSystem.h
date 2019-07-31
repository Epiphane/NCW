// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <RGBBinding/BindingProperty.h>
#include <Engine/Core/Timer.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/ParticleSystem.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"

namespace CubeWorld
{

//
// ParticleEmitter is a simple wrapper for an Engine::ParticleSystem
// as a component.
//
struct ParticleEmitter : public Engine::Component<ParticleEmitter>, public Engine::ParticleSystem {
public:
   //
   // Construction
   //
   ParticleEmitter(const std::string& path);

public:
   // Runtime configuration
   bool update = true;
   bool render = true;

   friend class SimpleParticleSystem;
};

//
// SimpleParticleSystem allows for the expression of multiple particle
// systems. For each ParticleSystem component
//
class SimpleParticleSystem : public Engine::System<SimpleParticleSystem> {
public:
   SimpleParticleSystem(Engine::Graphics::Camera* camera);
   ~SimpleParticleSystem();

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;

   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }

   bool IsPaused() { return mPause; }
   void SetPaused(bool paused) { mPause = paused; }
   
private:
   Engine::Graphics::Camera* mCamera;

   static std::unique_ptr<Engine::Graphics::Program> updater;

private:
   bool mDisabled;

   static constexpr uint16_t RANDOM_SIZE = 1000;
   uint32_t mTick;
   bool mPause = false;
   std::unique_ptr<Engine::Graphics::Texture> mRandom;

   std::unique_ptr<DebugHelper::MetricLink> mUpdateMetric;
   std::unique_ptr<DebugHelper::MetricLink> mRenderMetric;
   Engine::Timer<100> mUpdateClock;
   Engine::Timer<100> mRenderClock;
};

}; // namespace CubeWorld

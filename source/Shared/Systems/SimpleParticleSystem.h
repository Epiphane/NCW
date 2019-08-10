// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

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
};

//
// MultipleParticleEmitters is a wrapper for managing multiple
// particle systems attached to one entity.
//
struct MultipleParticleEmitters : public Engine::Component<MultipleParticleEmitters> {
   struct Emitter : public Engine::ParticleSystem {
      using Engine::ParticleSystem::ParticleSystem;

      //
      // If useEntityTransform is false, then the transform
      // matrix will be used for particle spawning instead.
      //
      bool useEntityTransform = true;

      glm::mat4 transform{1};
      bool update = true;
      bool render = true;
   };

   //
   // Each particle system is paired with a transform
   // in order to place it in the world.
   //
   std::vector<Emitter> systems;
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
   
private:
   Engine::Graphics::Camera* mCamera;

   //
   // Bind system-specific uniforms and runs the update once.
   // Does not bind the program; do that before calling this.
   //
   void UpdateParticleSystem(Engine::ParticleSystem& system) const;

   //
   // Bind system-specific uniforms and runs the render once.
   //
   void RenderParticleSystem(
      const glm::mat4& perspective,
      const glm::mat4& view,
      const glm::vec3& cameraPos,
      Engine::ParticleSystem& system
   ) const;

   static std::unique_ptr<Engine::Graphics::Program> updater;

private:
   bool mDisabled;

   static constexpr uint16_t RANDOM_SIZE = 1000;
   uint32_t mTick;
   std::unique_ptr<Engine::Graphics::Texture> mRandom;

   std::unique_ptr<DebugHelper::MetricLink> mUpdateMetric;
   std::unique_ptr<DebugHelper::MetricLink> mRenderMetric;
   Engine::Timer<100> mUpdateClock;
   Engine::Timer<100> mRenderClock;
};

}; // namespace CubeWorld

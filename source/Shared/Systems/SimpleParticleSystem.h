// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <RGBBinding/BindingProperty.h>
#include <Engine/Core/Timer.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"

namespace CubeWorld
{

//
// ParticleEmitter is a container for an entire particle system.
// It  has all the configuration properties for the particle system,
// including emission and particle properties.
//
struct ParticleEmitter : public Engine::Component<ParticleEmitter> {
public:
   // Types
   struct Particle
   {
      float type;
      glm::vec3 pos;
      glm::vec3 vel;
      float lifetime;
   };

   struct Options
   {
      std::string name;
      std::string vertexShader = "";
      std::string geometryShader = "";
      std::string fragmentShader = "";
   };

private:
   ParticleEmitter();

   void Initialize(const Options& options);

public:
   // Methods
   ParticleEmitter(const Options& options);
   ParticleEmitter(const std::string& dir, const BindingProperty& serialized);
   ParticleEmitter(const ParticleEmitter& other);
   ~ParticleEmitter();

   BindingProperty Serialize();

public:
   // Configuration
   double launcherLifetime;
   double particleLifetime;

   const std::string& GetName() const { return name; }

private:
   // Map of particle name to GL program
   static std::unordered_map<std::string, std::unique_ptr<Engine::Graphics::Program>> programs;

   std::string name;
   Engine::Graphics::Program* program;
   
private:
   // Particle system state
   friend class SimpleParticleSystem;
   bool firstRender;
   uint8_t buffer;
   Engine::Graphics::VBO particleBuffers[2];
   GLuint feedbackBuffers[2];
};

//
// SimpleParticleSystem allows for the expression of multiple particle
// systems. For each ParticleSystem component
//
class SimpleParticleSystem : public Engine::System<SimpleParticleSystem> {
public:
   SimpleParticleSystem(Engine::Graphics::Camera* camera = nullptr);
   ~SimpleParticleSystem();

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;

   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }
   
private:
   Engine::Graphics::Camera* mCamera;

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

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
   enum class Shape : uint32_t
   {
      Point = 0,
      Cone = 1,
   };

   // Types
   struct Particle
   {
      float type = 0;
      glm::vec3 pos = {0, 0, 0};
      glm::vec4 rot = {0, 0, 1, 0};
      glm::vec3 vel = {0, 0, 0};
      float age = 0;
   };

   struct Options
   {
      std::string name;
      uint64_t maxParticles = 1000;
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
   float emitterCooldown;
   float spawnAge[2];
   float particleLifetime;

   // Shape configuration
   Shape shape;
   glm::vec3 shapeParam0;
   float shapeParam1;
   float shapeParam2;
   float shapeParam3;

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

   bool IsPaused() { return mPause; }
   void SetPaused(bool paused) { mPause = paused; }
   
private:
   Engine::Graphics::Camera* mCamera;

   static std::unique_ptr<Engine::Graphics::Program> updater;

private:
   bool mDisabled;

   static constexpr uint16_t RANDOM_SIZE = 1000;
   uint32_t mTick;
   bool mPause;
   std::unique_ptr<Engine::Graphics::Texture> mRandom;

   std::unique_ptr<DebugHelper::MetricLink> mUpdateMetric;
   std::unique_ptr<DebugHelper::MetricLink> mRenderMetric;
   Engine::Timer<100> mUpdateClock;
   Engine::Timer<100> mRenderClock;
};

}; // namespace CubeWorld

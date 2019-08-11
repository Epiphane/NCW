// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <RGBBinding/BindingProperty.h>
#include "Program.h"
#include "TextureManager.h"
#include "VBO.h"

namespace CubeWorld
{

namespace Engine
{

//
// ParticleSystem is a container for an entire particle system.
// It has all the configuration properties for the particle system,
// including emission and particle properties, and stores buffers
// with the current state of the system.
//
// It's designed similar to a component, but not as a component.
// This adds flexibility for particle systems to be added to the
// game in multiple ways, however they best fit.
//
struct ParticleSystem {
public:
   //
   // Types
   //
   enum class Shape : uint32_t
   {
      Point = 0,
      Cone = 1,
      Trail = 2,
   };

   struct Particle
   {
      float type = 0;
      glm::vec3 pos = {0, 0, 0};
      glm::vec4 rot = {0, 0, 1, 0};
      glm::vec3 vel = {0, 0, 0};
      float age = 0;
   };

private:
   ParticleSystem();
   void Initialize(
      const BindingProperty& config,
      const std::string& shaderDir,
      const std::string& textureDir
   );

public:
   // Methods
   ParticleSystem(
      const std::string& configPath,
      const std::string& shaderDir,
      const std::string& textureDir
   );
   ParticleSystem(
      const BindingProperty& config,
      const std::string& shaderDir,
      const std::string& textureDir
   );
   ParticleSystem(ParticleSystem&& other);
   ~ParticleSystem();

   //
   // Reset the particle system to an initial state,
   // with just one emitter.
   //
   void Reset();

   //
   // Apply configurations to the system. Will only
   // override provided configs, not changing anything
   // that is not explicitly specified.
   //
   void ApplyConfiguration(
      const std::string& textureDir,
      const BindingProperty& config
   );

   //
   // Serialize all configuration into a BindingProperty.
   //
   BindingProperty Serialize();

   const std::string& GetName() const { return name; }

public:
   //
   // Maximum amount of particles supported. Changes 
   // will not take effect until the next Reset().
   //
   uint32_t maxParticles = 1000;

   //
   // Amount of time between particle spawns.
   // Equal to 1 / {particles per second}
   //
   float emitterCooldown = 1.0f;

   //
   // Lifetime of the entire system.
   // 0 means run indefinitely.
   //
   float emitterLifetime = 0.0f;

   //
   // Useful for tracking (without referencing the VBO)
   // the current state of the emitter
   //
   float age = 0.0f;

   //
   // Min/max age to spawn particles at, for
   // variability.
   //
   float spawnAge[2] = {0, 0};

   //
   // Lifetime of each particle. Use spawnAge
   // to configure variable lifetimes.
   //
   float particleLifetime = 0.0f;

   //
   // Shape in which to emit particles.
   //
   Shape shape = Shape::Point;

   //
   // Variable params, applied differently to
   // each shape and thus not given names.
   //
   glm::vec3 shapeParam0 = {0, 0, 0};
   float shapeParam1 = 0;
   float shapeParam2 = 0;
   float shapeParam3 = 0;

   //
   // Provided texture for the renderer.
   //
   Graphics::Texture* texture = nullptr;
   
   //
   // Renderer.
   //
   Graphics::Program* program;
   
   //
   // Custom uniforms to apply to the renderer.
   //
   BindingProperty uniforms;

protected:
   //
   // Map of particle name to GL program
   //
   static std::unordered_map<std::string, std::unique_ptr<Graphics::Program>> programs;

   //
   // Name of the particle system.
   //
   std::string name;
   
public:
   //
   // Internal state.
   //
   bool firstRender = true;
   uint8_t buffer = 0;
   Graphics::VBO particleBuffers[2];
   GLuint feedbackBuffers[2] = {0, 0};
};

}; // namespace Engine

}; // namespace CubeWorld

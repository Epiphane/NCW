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
   ~ParticleSystem();

   BindingProperty Serialize();

   const std::string& GetName() const { return name; }

public:
   //
   // Amount of time between particle spawns.
   // Equal to 1 / {particles per second}
   //
   float emitterCooldown = 0.0f;

   //
   // Min/max age to spawn particles at, for
   // variability.
   //
   float spawnAge[2];

   //
   // Lifetime of each particle. Use spawnAge
   // to configure variable lifetimes.
   //
   float particleLifetime;

   //
   // Shape in which to emit particles.
   //
   Shape shape;

   //
   // Variable params, applied differently to
   // each shape and thus not given names.
   //
   glm::vec3 shapeParam0;
   float shapeParam1;
   float shapeParam2;
   float shapeParam3;

   //
   // Provided texture for the renderer.
   //
   Graphics::Texture* texture = nullptr;
   
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
   
   //
   // Renderer.
   //
   Graphics::Program* program;
   
protected:
   //
   // Internal state.
   //
   friend class SimpleParticleSystem;
   bool firstRender = true;
   uint8_t buffer = 0;
   Graphics::VBO particleBuffers[2];
   GLuint feedbackBuffers[2] = {0, 0};
};

}; // namespace Engine

}; // namespace CubeWorld

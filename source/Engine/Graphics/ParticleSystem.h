// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Meta.h>

#include <RGBBinding/BindingProperty.h>
#include <RGBMeta/Value.h>
#include "Program.h"
#include "TextureManager.h"
#include "VBO.h"
#include "FeedbackBuffer.h"

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
   struct Particle
   {
      float type = 0;
      glm::vec3 pos = {0, 0, 0};
      glm::vec4 rot = {0, 0, 1, 0};
      glm::vec3 vel = {0, 0, 0};
      float age = 0;
   };

   struct LauncherConfig
   {
   public:
      void SetPPS(float particlesPerSecond) {
         cooldown = 1.0f / particlesPerSecond;
      }
      float GetPPS() const {
         return 1.0f / cooldown;
      }

   public:
      //
      // Amount of time between particle spawns.
      // Equal to 1 / {particles per second}
      //
      float cooldown = 1.0f;

      //
      // Lifetime of the entire system.
      // 0 means run indefinitely.
      //
      float lifetime = 0.0f;
   };

   struct MinMax
   {
      float min = 0;
      float max = 0;
   };

   struct ParticleConfig
   {

      //
      // Min/max age to spawn particles at, for
      // variability.
      //
      MinMax spawnAge;

      //
      // Lifetime of each particle. Use spawnAge
      // to configure variable lifetimes.
      //
      float lifetime = 0.0f;
   };

   //
   // Shape configurations
   //
   enum class Shape : uint32_t
   {
      Point = 0,
      Cone = 1,
      Trail = 2,
   };

   struct PointConfig
   {
      // Empty
   };

   struct ConeConfig
   {
      glm::vec3 direction = {0, 0, 0};
      float radius = 0.0f;
      MinMax height;
   };

   struct TrailConfig
   {
      // Empty
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
   ParticleSystem(const ParticleSystem& other) = default;
   ParticleSystem(ParticleSystem&& other) = default;
   virtual ~ParticleSystem() = default;

   ParticleSystem& operator=(const ParticleSystem& other) = default;

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
   void ApplyConfiguration(const std::string& textureDir, const BindingProperty& config);

   //
   // Serialize all configuration into a BindingProperty.
   //
   BindingProperty Serialize() const;

   void SetTexture(const std::string& texture);
   const std::string& GetTexture() const;

   const inline bool IsPoint() const;
   const inline bool IsCone()  const;
   const inline bool IsTrail() const;
   const inline PointConfig& GetPoint() const;
   const inline ConeConfig& GetCone()   const;
   const inline TrailConfig& GetTrail() const;
   inline void SetPoint(const PointConfig& point);
   inline void SetCone(const ConeConfig& cone);
   inline void SetTrail(const TrailConfig& trail);

public:
   //
   // Name of the particle system.
   //
   std::string name;

   //
   // Shader name.
   //
   std::string shader;

   //
   // Maximum amount of particles supported. Changes
   // will not take effect until the next Reset().
   //
   uint32_t maxParticles = 1000;

   //
   // Launcher configuration.
   //
   LauncherConfig launcher;

   //
   // Particle configuration
   //
   ParticleConfig particle;

   //
   // Shape in which to emit particles.
   //
   Shape shape = Shape::Point;

   union {
      PointConfig point;
      ConeConfig cone;
      TrailConfig trail;
   } shapeConfig;

   //
   // Renderer.
   //
   Graphics::Program* program;

   //
   // Custom uniforms to apply to the renderer.
   //
   BindingProperty uniforms;

   //
   // Rendered texture.
   //
   std::string textureName;

public:
   //
   // Useful for tracking (without referencing the VBO)
   // the current state of the emitter
   //
   float age = 0.0f;

   //
   // Location in which to find textures.
   //
   std::string textureDirectory;

   //
   // Provided texture for the renderer.
   //
   Graphics::Texture* texture = nullptr;

protected:
   //
   // Map of particle name to GL program
   //
   static std::unordered_map<std::string, std::unique_ptr<Graphics::Program>> programs;

public:
   //
   // Internal state.
   //
   bool firstRender = true;
   uint8_t buffer = 0;
   Graphics::FeedbackBuffer buffers[2];

   //
   // For particle systems that manage their data without the GPU (trails).
   //
   std::vector<Particle> particles;
};

}; // namespace Engine

}; // namespace CubeWorld

namespace meta {

using CubeWorld::Engine::ParticleSystem;

template <>
inline auto registerMembers<ParticleSystem::LauncherConfig>()
{
   return members(
      member("particles-per-second", &ParticleSystem::LauncherConfig::GetPPS, &ParticleSystem::LauncherConfig::SetPPS),
      member("lifetime", &ParticleSystem::LauncherConfig::lifetime)
   );
}

template <>
inline auto registerMembers<ParticleSystem::MinMax>()
{
   return members(
      member("min", &ParticleSystem::MinMax::min),
      member("max", &ParticleSystem::MinMax::max)
   );
}

template <>
inline auto registerMembers<ParticleSystem::ParticleConfig>()
{
   return members(
      member("spawn-age", &ParticleSystem::ParticleConfig::spawnAge),
      member("lifetime", &ParticleSystem::ParticleConfig::lifetime)
   );
}

template <>
inline auto registerValues<ParticleSystem::Shape>()
{
   return values(
      value("cone", ParticleSystem::Shape::Cone),
      value("point", ParticleSystem::Shape::Point),
      value("trail", ParticleSystem::Shape::Trail)
   );
}

template <>
inline auto registerMembers<ParticleSystem::PointConfig>()
{
   return members();
}

template <>
inline auto registerMembers<ParticleSystem::ConeConfig>()
{
   return members(
      member("direction", &ParticleSystem::ConeConfig::direction),
      member("radius", &ParticleSystem::ConeConfig::radius),
      member("height", &ParticleSystem::ConeConfig::height)
   );
}

template <>
inline auto registerMembers<ParticleSystem::TrailConfig>()
{
   return members();
}

template <>
inline auto registerMembers<ParticleSystem>()
{
   return members(
      member("name", &ParticleSystem::name),
      member("shader", &ParticleSystem::shader),
      member("max-particles", &ParticleSystem::maxParticles),
      member("shader-uniforms", &ParticleSystem::uniforms),
      member("shader-texture", &ParticleSystem::GetTexture, &ParticleSystem::SetTexture),
      member("launcher", &ParticleSystem::launcher),
      member("particle", &ParticleSystem::particle),
      member("shape", &ParticleSystem::shape),
      //member("point", &ParticleSystem::GetPoint, &ParticleSystem::SetPoint, &ParticleSystem::IsPoint),
      //member("trail", &ParticleSystem::GetTrail, &ParticleSystem::SetTrail, &ParticleSystem::IsTrail),
      member("cone", &ParticleSystem::GetCone, &ParticleSystem::SetCone, &ParticleSystem::IsCone)
   );
}

}; // namespace meta

namespace CubeWorld
{

namespace Engine
{

const inline bool ParticleSystem::IsPoint() const { return shape == Shape::Point; }
const inline bool ParticleSystem::IsCone()  const { return shape == Shape::Cone; }
const inline bool ParticleSystem::IsTrail() const { return shape == Shape::Trail; }
const inline ParticleSystem::PointConfig& ParticleSystem::GetPoint() const { return shapeConfig.point; }
const inline ParticleSystem::ConeConfig& ParticleSystem::GetCone()   const { return shapeConfig.cone; }
const inline ParticleSystem::TrailConfig& ParticleSystem::GetTrail() const { return shapeConfig.trail; }
inline void ParticleSystem::SetPoint(const PointConfig& point) { shape = Shape::Point; shapeConfig.point = point; }
inline void ParticleSystem::SetCone(const ConeConfig& cone) { shape = Shape::Cone;  shapeConfig.cone = cone; }
inline void ParticleSystem::SetTrail(const TrailConfig& trail) { shape = Shape::Trail; shapeConfig.trail = trail; }

}; // namespace Engine

}; // namespace CubeWorld

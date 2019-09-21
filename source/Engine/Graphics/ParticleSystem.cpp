// By Thomas Steinke

#include <glad/glad.h>
#include <glm/ext.hpp>

#include <RGBDesignPatterns/Scope.h>
#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>

#include "ParticleSystem.h"

namespace CubeWorld
{

namespace Engine
{

// ------------------------------------------------------------------------------------------------
// |                                                                                              |
// |                                     ParticleSystem                                           |
// |                                                                                              |
// ------------------------------------------------------------------------------------------------
std::unordered_map<std::string, std::unique_ptr<Engine::Graphics::Program>> ParticleSystem::programs;

ParticleSystem::ParticleSystem()
   : program(nullptr)
   , name("")
   , firstRender(true)
   , buffer(0)
   , particleBuffers{Engine::Graphics::VBO::Vertices, Engine::Graphics::VBO::Vertices}
{
   glGenTransformFeedbacks(2, feedbackBuffers);

   // Set up feedback buffers
   for (size_t i = 0; i < 2; ++i)
   {
      glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackBuffers[i]);
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffers[i].GetBuffer());
   }
}

ParticleSystem::ParticleSystem(ParticleSystem&& other) noexcept
   : maxParticles(other.maxParticles)
   , emitterCooldown(other.emitterCooldown)
   , emitterLifetime(other.emitterLifetime)
   , age{other.age}
   , spawnAge{other.spawnAge[0], other.spawnAge[1]}
   , particleLifetime(other.particleLifetime)
   , shape(other.shape)
   , shapeParam0(other.shapeParam0)
   , shapeParam1(other.shapeParam1)
   , shapeParam2(other.shapeParam2)
   , shapeParam3(other.shapeParam3)
   , texture(other.texture)
   , program(other.program)
   , uniforms(std::move(other.uniforms))
   , name(std::move(other.name))
   , firstRender(other.firstRender)
   , buffer(other.buffer)
   , particleBuffers{std::move(other.particleBuffers[0]), std::move(other.particleBuffers[1])}
   , feedbackBuffers{other.feedbackBuffers[0], other.feedbackBuffers[1]}
   , particles{std::move(other.particles)}
{
   other.feedbackBuffers[0] = other.feedbackBuffers[1] = 0;
}

ParticleSystem::~ParticleSystem()
{
   if (feedbackBuffers[0] != 0 || feedbackBuffers[1] != 0)
   {
      glDeleteTransformFeedbacks(2, feedbackBuffers);
   }
}

ParticleSystem::ParticleSystem(
   const std::string& path,
   const std::string& shaderDir,
   const std::string& textureDir
) : ParticleSystem()
{
   Maybe<BindingProperty> data = YAMLSerializer::DeserializeFile(path);
   assert(data.Succeeded());
   if (!data)
   {
      data.Failure().WithContext("Failed loading {path}", path).Log();
      return;
   }

   Initialize(*data, shaderDir, textureDir);
}

ParticleSystem::ParticleSystem(
   const BindingProperty& config,
   const std::string& shaderDir,
   const std::string& textureDir
) : ParticleSystem()
{
   Initialize(config, shaderDir, textureDir);
}

void ParticleSystem::Initialize(
   const BindingProperty& config,
   const std::string& shaderDir,
   const std::string& textureDir
)
{
   name = config["name"];

   shader = config["shader"];
   std::string vertexShader = Paths::Join(shaderDir, shader + ".vert");
   std::string geometryShader = Paths::Join(shaderDir, shader + ".geom");
   std::string fragmentShader = Paths::Join(shaderDir, shader + ".frag");

   auto it = programs.find(name);
   if (it == programs.end())
   {
      auto maybeProgram = Engine::Graphics::Program::Load(
         vertexShader,
         geometryShader,
         fragmentShader
      );
      if (!maybeProgram)
      {
         maybeProgram.Failure().WithContext("Failed loading particle shader for {name}", name).Log();
         return;
      }
      else
      {
         programs[name] = std::move(*maybeProgram);
         program = programs[name].get();
      }
   }
   else
   {
      program = it->second.get();
   }

   //
   // Read particle system configuration.
   //
   ApplyConfiguration(textureDir, config);

   //
   // Set up initial particle.
   //
   maxParticles = config["max-particles"].GetUintValue(1000);
   Reset();
}

void ParticleSystem::Reset()
{
   age = 0;
   buffer = 0;
   firstRender = true;

   //
   // Initialize one emitter particle
   //
   if (shape == Shape::Trail)
   {
      // Create lead particles
      particles.resize(2);
      particles[0].type = 2.0f; // Particle
      particles[0].pos = {0,0,0};
      particles[0].rot = {1,0,0,0}; // Top
      particles[0].age = 0;
      particles[1].type = 2.0f; // Particle
      particles[1].pos = {0,0,0};
      particles[1].rot = {0,1,0,0}; // Bottom
      particles[1].age = 0;
      particleBuffers[0].BufferData(sizeof(Particle) * particles.size(), particles.data(), GL_STATIC_DRAW);
   }
   else
   {
      std::vector<Particle> data;
      data.resize(maxParticles);
      data[0].type = 1.0f; // Emitter
      data[0].age = emitterCooldown; // Emit immediately
      particleBuffers[0].BufferData(sizeof(Particle) * data.size(), data.data(), GL_STATIC_DRAW);
      particleBuffers[1].BufferData(sizeof(Particle) * data.size(), data.data(), GL_STATIC_DRAW);
   }
}

BindingProperty ParticleSystem::Serialize() const
{
   BindingProperty result;

   result["name"] = name;
   result["shader"] = shader;
   result["launcher"]["particles-per-second"] = 1.0 / emitterCooldown;
   result["launcher"]["lifetime"] = emitterLifetime;
   result["particle"]["lifetime"] = particleLifetime;
   result["particle"]["spawn-age"]["min"] = spawnAge[0];
   result["particle"]["spawn-age"]["max"] = spawnAge[1];
   result["max-particles"] = maxParticles;
   result["shader-uniforms"] = uniforms;
   
   if (!textureName.empty())
   {
      result["shader-texture"] = textureName;
   }

   switch (shape)
   {
   case Shape::Cone:
   {
      BindingProperty& cone = result["cone"];
      result["shape"] = "cone";
      cone["direction"] = shapeParam0;
      cone["radius"] = shapeParam1;
      cone["height"]["min"] = shapeParam2;
      cone["height"]["max"] = shapeParam3;
      break;
   }

   case Shape::Trail:
   {
      result["shape"] = "trail";
      break;
   }

   case Shape::Point:
   default:
   {
      result["shape"] = "point";
      break;
   }
   }

   return result;
}

void ParticleSystem::ApplyConfiguration(
   const std::string& textureDir,
   const BindingProperty& config
)
{
   emitterCooldown = 1.0f / config["launcher"]["particles-per-second"].GetFloatValue(1.0f / emitterCooldown);
   emitterLifetime = config["launcher"]["lifetime"].GetFloatValue(emitterLifetime);
   particleLifetime = config["particle"]["lifetime"].GetFloatValue(particleLifetime);
   spawnAge[0] = config["particle"]["spawn-age"]["min"].GetFloatValue(spawnAge[0]);
   spawnAge[1] = config["particle"]["spawn-age"]["max"].GetFloatValue(spawnAge[1]);

   if (config.Has("shader-uniforms"))
   {
      uniforms = config["shader-uniforms"];
   }

   textureName = config["shader-texture"];
   if (!textureName.empty())
   {
      Maybe<Engine::Graphics::Texture*> maybeTexture = Engine::Graphics::TextureManager::Instance().GetTexture(Paths::Join(textureDir, textureName));
      if (!maybeTexture)
      {
         maybeTexture.Failure().WithContext("Failed loading {texture}", textureName).Log();
      }
      else
      {
         texture = *maybeTexture;
      }
   }

   if (config.Has("shape"))
   {
      std::string shapeVal = config["shape"];
      if (shapeVal == "point")
      {
         shape = Shape::Point;
      }
      else if (shapeVal == "cone")
      {
         shape = Shape::Cone;
      }
      else if (shapeVal == "trail")
      {
         shape = Shape::Trail;
      }
      else
      {
         LOG_ERROR("Unknown shape value {shape}. Defaulting to point", shapeVal);
         shape = Shape::Point;
      }
   }

   if (shape == Shape::Point || shape == Shape::Trail)
   {
      shapeParam0 = {0, 0, 0};
      shapeParam1 = 0;
      shapeParam2 = 0;
   }
   else if (shape == Shape::Cone)
   {
      const BindingProperty& cone = config["cone"];
      shapeParam0 = cone["direction"].GetVec3(shapeParam0);
      shapeParam1 = cone["radius"].GetFloatValue(shapeParam1);
      shapeParam2 = cone["height"]["min"].GetFloatValue(shapeParam2);
      shapeParam3 = cone["height"]["max"].GetFloatValue(shapeParam3);
   }
}

}; // namespace Engine

}; // namespace CubeWorld

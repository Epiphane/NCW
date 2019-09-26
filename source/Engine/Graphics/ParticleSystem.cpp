// By Thomas Steinke

#include <glad/glad.h>
#include <glm/ext.hpp>

#include <RGBBinding/BindingPropertyMeta.h>
#include <RGBDesignPatterns/Scope.h>
#include <RGBDesignPatterns/Macros.h>
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
   : name("")
   , shapeConfig{PointConfig{}}
   , program(nullptr)
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
   : name(std::move(other.name))
   , maxParticles(other.maxParticles)
   , launcher(other.launcher)
   , particle(other.particle)
   , shape(other.shape)
   , shapeConfig(other.shapeConfig)
   , program(other.program)
   , uniforms(std::move(other.uniforms))
   , age{other.age}
   , textureDirectory{other.textureDirectory}
   , texture(other.texture)
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
      data[0].age = launcher.cooldown; // Emit immediately
      particleBuffers[0].BufferData(sizeof(Particle) * data.size(), data.data(), GL_STATIC_DRAW);
      particleBuffers[1].BufferData(sizeof(Particle) * data.size(), data.data(), GL_STATIC_DRAW);
   }
}

void ParticleSystem::SetTexture(const std::string& tex)
{
   textureName = tex;
   if (!textureName.empty())
   {
      Maybe<Engine::Graphics::Texture*> maybeTexture = Engine::Graphics::TextureManager::Instance().GetTexture(Paths::Join(textureDirectory, textureName));
      if (!maybeTexture)
      {
         maybeTexture.Failure().WithContext("Failed loading {texture}", textureName).Log();
      }
      else
      {
         texture = *maybeTexture;
      }
   }
}

const std::string& ParticleSystem::GetTexture() const
{
   return textureName;
}

void ParticleSystem::ApplyConfiguration(const std::string& textureDir, const BindingProperty& config)
{
   textureDirectory = textureDir;
   deserialize(*this, config);

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

   if (shape == Shape::Cone && config.Has("cone"))
   {
      deserialize(shapeConfig.cone, config["cone"]);
   }
}

BindingProperty ParticleSystem::Serialize() const
{
   BindingProperty result(*this);

   switch (shape)
   {
   case Shape::Cone:
   {
      result["shape"] = "cone";
      result["cone"] = shapeConfig.cone;
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

}; // namespace Engine

}; // namespace CubeWorld

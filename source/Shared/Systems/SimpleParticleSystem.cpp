// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <glad/glad.h>
#include <glm/ext.hpp>

#include <RGBDesignPatterns/Scope.h>
#include <Engine/Core/Timer.h>
#include <Engine/Graphics/Program.h>
#include <RGBLogger/Logger.h>

#include "SimpleParticleSystem.h"

namespace CubeWorld
{

std::unordered_map<std::string, std::unique_ptr<Engine::Graphics::Program>> ParticleEmitter::programs;

ParticleEmitter::ParticleEmitter(const Options& options)
   : name(options.name)
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
      // particleBuffers[i].Bind();
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffers[i].GetBuffer());
   }

   auto it = programs.find(options.name);
   if (it == programs.end())
   {
      auto maybeProgram = Engine::Graphics::Program::Load(
         options.vertexShader,
         options.geometryShader,
         options.fragmentShader
      );
      if (!maybeProgram)
      {
         LOG_ERROR("%1", maybeProgram.Failure().WithContext("Failed loading particle shader").GetMessage());
      }
      else
      {
         programs.emplace(name, std::move(*maybeProgram));
         program = programs.at(name).get();
      }
   }
   else
   {
      program = it->second.get();
   }
}

ParticleEmitter::ParticleEmitter(const ParticleEmitter& other)
   : ParticleEmitter(Options{other.name})
{
}

ParticleEmitter::~ParticleEmitter()
{
   glDeleteTransformFeedbacks(2, feedbackBuffers);
}

std::unique_ptr<Engine::Graphics::Program> SimpleParticleSystem::updater = nullptr;

SimpleParticleSystem::SimpleParticleSystem(Engine::Graphics::Camera* camera) : mCamera(camera)
{}

SimpleParticleSystem::~SimpleParticleSystem()
{}

void SimpleParticleSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
   mUpdateMetric = DebugHelper::Instance().RegisterMetric("Particle Update Time", [this]() -> std::string {
      return Format::FormatString("%.2fms", mUpdateClock.Average() * 1000.0);
   });
   mRenderMetric = DebugHelper::Instance().RegisterMetric("Particle Render Time", [this]() -> std::string {
      return Format::FormatString("%.2fms", mRenderClock.Average() * 1000.0);
   });

   mTick = 0;

   if (!updater)
   {
      auto maybeUpdater = Engine::Graphics::Program::Load(
         "Shaders/ParticleSystem.vert",
         "Shaders/ParticleSystem.geom",
         "Shaders/ParticleSystem.frag",
         {"fType", "fPosition", "fVelocity", "fAge"}
      );
      if (!maybeUpdater)
      {
         LOG_ERROR(maybeUpdater.Failure().WithContext("Failed loading ParticleSystem shader").GetMessage());
      }
      else
      {
         updater = std::move(*maybeUpdater);
         updater->Attrib("aPosition");
         updater->Attrib("aColor");
         updater->Uniform("uRandomTexture");
         updater->Uniform("uViewMatrix");
         updater->Uniform("uModelMatrix");
      }
   }

   if (!mRandom)
   {
      auto maybeTexture = Engine::Graphics::Texture::MakeRandom(RANDOM_SIZE);
      if (!maybeTexture)
      {
         LOG_ERROR(maybeTexture.Failure().WithContext("Failed generating random texture").GetMessage());
      }
      else
      {
         mRandom = std::move(*maybeTexture);
      }
   }

   const GLubyte *rend = glGetString(GL_RENDERER);
   const char* found = strstr((const char*)rend, "AMD");
   mDisabled = (found != nullptr);
   if (mDisabled)
   {
      LOG_WARNING("Particle rendering disabled due to unsupported graphics renderer");
   }
}

using Transform = Engine::Transform;

void SimpleParticleSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   if (++mTick >= RANDOM_SIZE)
   {
      mTick = 0;
   }

   // Update and render can be in one loop but this helps with performance measurements
   mUpdateClock.Reset();
   {
      BIND_PROGRAM_IN_SCOPE(updater);
      entities.Each<ParticleEmitter>([&](Engine::Entity /*entity*/, ParticleEmitter& emitter) {
         glEnable(GL_RASTERIZER_DISCARD);
         CUBEWORLD_SCOPE_EXIT([&]{ glDisable(GL_RASTERIZER_DISCARD); });

         if (mRandom)
         {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mRandom->GetTexture());
            updater->Uniform1i("uRandomTexture", 0);
         }

         updater->Uniform1f("uLauncherLifetime", 8.0f);
         updater->Uniform1f("uShellLifetime", 8.0f);
         updater->Uniform1f("uDeltaTimeMillis", (float)dt);
         updater->Uniform1f("uTick", (float)mTick);

         emitter.particleBuffers[emitter.buffer].Bind();
         glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, emitter.feedbackBuffers[1 - emitter.buffer]);
         
         // We enable the attribute pointers individually since a normal VBO only
         // expects to be attached to one.
         glEnableVertexAttribArray(0);
         glEnableVertexAttribArray(1);
         glEnableVertexAttribArray(2);
         glEnableVertexAttribArray(3);

         glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), 0);                   // type
         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)4);    // position
         glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)16);   // velocity
         glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)28);   // lifetime

         // Begin rendering
         glBeginTransformFeedback(GL_POINTS);

         if (emitter.firstRender) {
            glDrawArrays(GL_POINTS, 0, 1);
            emitter.firstRender = false;
         }
         else {
            glDrawTransformFeedback(GL_POINTS, emitter.feedbackBuffers[emitter.buffer]);
         }

         // Flip buffers
         emitter.buffer = uint8_t(1 - emitter.buffer);

         glEndTransformFeedback();

         glDisableVertexAttribArray(0);
         glDisableVertexAttribArray(1);
         glDisableVertexAttribArray(2);
         glDisableVertexAttribArray(3);

         CHECK_GL_ERRORS();
      });
   }
   mUpdateClock.Elapsed();

   mRenderClock.Reset();
   entities.Each<ParticleEmitter>([&](Engine::Entity /*entity*/, ParticleEmitter& emitter) {
      // Render
      if (emitter.program != nullptr)
      {
         BIND_PROGRAM_IN_SCOPE(emitter.program);

         glm::mat4 perspective = mCamera->GetPerspective();
         glm::mat4 view = mCamera->GetView();
         emitter.program->UniformMatrix4f("uProjMatrix", perspective);
         emitter.program->UniformMatrix4f("uViewMatrix", view);

         emitter.particleBuffers[emitter.buffer].Bind();
         
         // We enable the attribute pointers individually since a normal VBO only
         // expects to be attached to one.
         glEnableVertexAttribArray(0);
         glEnableVertexAttribArray(1);
         glEnableVertexAttribArray(2);
         // glEnableVertexAttribArray(3);

         glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), 0);                   // type
         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)4);    // position
         // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)16);   // velocity
         // glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)28);   // lifetime
         glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)28);   // lifetime

         glDrawTransformFeedback(GL_POINTS, emitter.feedbackBuffers[emitter.buffer]);

         glDisableVertexAttribArray(0);
         glDisableVertexAttribArray(1);
         glDisableVertexAttribArray(2);
         // glDisableVertexAttribArray(3);

         CHECK_GL_ERRORS();

      }
   });
   mRenderClock.Elapsed();
}

}; // namespace CubeWorld

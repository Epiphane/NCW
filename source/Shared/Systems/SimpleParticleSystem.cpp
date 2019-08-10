// By Thomas Steinke

#include <glad/glad.h>
#include <glm/ext.hpp>

#include <RGBDesignPatterns/Scope.h>
#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>

#include "../Helpers/Asset.h"
#include "SimpleParticleSystem.h"

namespace CubeWorld
{

// ------------------------------------------------------------------------------------------------
// |                                                                                              |
// |                                     ParticleEmitter                                          |
// |                                                                                              |
// ------------------------------------------------------------------------------------------------
ParticleEmitter::ParticleEmitter(const std::string& path)
   : Engine::ParticleSystem(path, Asset::ParticleShaders(), Asset::Image(""))
   , update(true)
   , render(true)
{
}

// ------------------------------------------------------------------------------------------------
// |                                                                                              |
// |                                  SimpleParticleSystem                                        |
// |                                                                                              |
// ------------------------------------------------------------------------------------------------
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
         {"fType", "fPosition", "fRotation", "fVelocity", "fAge"}
      );
      if (!maybeUpdater)
      {
         LOG_ERROR(maybeUpdater.Failure().WithContext("Failed loading ParticleSystem shader").GetMessage());
      }
      else
      {
         updater = std::move(*maybeUpdater);
         updater->Attrib("aPosition");
         updater->Attrib("aRotation");
         updater->Attrib("aColor");
         updater->Uniform("uModelMatrix");
         updater->Uniform("uEmitterCooldown");
         updater->Uniform("uParticleLifetime");
         updater->Uniform("uDeltaTimeMillis");
         updater->Uniform("uTick");
         updater->Uniform("uShape");
         updater->Uniform("uSpawnAge");
         updater->Uniform("uShapeParam0");
         updater->Uniform("uShapeParam1");
         updater->Uniform("uShapeParam2");
         updater->Uniform("uShapeParam3");
         CHECK_GL_ERRORS();
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
      glEnable(GL_RASTERIZER_DISCARD);
      CUBEWORLD_SCOPE_EXIT([&] { glDisable(GL_RASTERIZER_DISCARD); });
      if (mRandom)
      {
         glActiveTexture(GL_TEXTURE3);
         glBindTexture(GL_TEXTURE_1D, mRandom->GetTexture());
         updater->Uniform1i("uRandomTexture", 3);
      }
      updater->Uniform1f("uDeltaTimeMillis", (float)dt);
      updater->Uniform1f("uTick", (float)mTick);

      entities.Each<Transform, ParticleEmitter>([&](Transform& transform, ParticleEmitter& emitter) {
         if (!emitter.update)
         {
            return;
         }

         updater->UniformMatrix4f("uModelMatrix", transform.GetMatrix());
         UpdateParticleSystem(emitter);
      });

      entities.Each<Transform, MultipleParticleEmitters>([&](Transform& transform, MultipleParticleEmitters& group) {
         for (auto& system : group.systems)
         {
            if (!system.update)
            {
               return;
            }

            if (system.useEntityTransform)
            {
               updater->UniformMatrix4f("uModelMatrix", transform.GetMatrix());
            }
            else
            {
               updater->UniformMatrix4f("uModelMatrix", system.transform);
            }
            UpdateParticleSystem(system);
         }
      });
   }
   mUpdateClock.Elapsed();

   mRenderClock.Reset();
   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   glm::vec3 position = mCamera->GetPosition();
   entities.Each<ParticleEmitter>([&](ParticleEmitter& emitter) {
      // Render
      if (emitter.render)
      {
         RenderParticleSystem(
            perspective,
            view,
            position,
            emitter
         );
      }
   });

   entities.Each<MultipleParticleEmitters>([&](MultipleParticleEmitters& group) {
      for (auto& system : group.systems)
      {
         if (system.render)
         {
            RenderParticleSystem(
               perspective,
               view,
               position,
               system
            );
         }
      }
   });
   mRenderClock.Elapsed();
}

void SimpleParticleSystem::UpdateParticleSystem(Engine::ParticleSystem& system) const
{
   updater->Uniform1f("uEmitterCooldown", system.emitterCooldown);
   updater->Uniform1f("uParticleLifetime", system.particleLifetime);

   updater->Uniform1u("uShape", (uint32_t)system.shape);
   updater->Uniform2f("uSpawnAge", system.spawnAge[0], system.spawnAge[1]);
   updater->UniformVector3f("uShapeParam0", system.shapeParam0);
   updater->Uniform1f("uShapeParam1", system.shapeParam1);
   updater->Uniform1f("uShapeParam2", system.shapeParam2);
   updater->Uniform1f("uShapeParam3", system.shapeParam3);

   system.particleBuffers[system.buffer].Bind();

   // We enable the attribute pointers individually since a normal VBO only
   // expects to be attached to one.
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);
   glEnableVertexAttribArray(3);
   glEnableVertexAttribArray(4);

   glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), 0);                   // type
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)4);    // position
   glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)16);    // rotation
   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)32);   // velocity
   glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)44);   // lifetime

   // GLuint drawnQuery;
   // int result = 0;
   // glGenQueries(1, &drawnQuery);
   // glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, drawnQuery);
   // Begin rendering
   if (system.firstRender) {
      glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, system.feedbackBuffers[1]);
      glBeginTransformFeedback(GL_POINTS);
      glDrawArrays(GL_POINTS, 0, 1);
      glEndTransformFeedback();

      // [Mac] Initialize both feedback buffers by rendering initial data into them
      // TODO is this even necessary?
      system.particleBuffers[1 - system.buffer].Bind();
      glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), 0);                   // type
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)4);    // position
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)16);    // rotation
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)32);   // velocity
      glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)44);   // lifetime
      glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, system.feedbackBuffers[0]);
      glBeginTransformFeedback(GL_POINTS);
      glDrawArrays(GL_POINTS, 0, 1);
      glEndTransformFeedback();

      system.firstRender = false;
   }
   else {
      glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, system.feedbackBuffers[1 - system.buffer]);
      glBeginTransformFeedback(GL_POINTS);
      glDrawTransformFeedback(GL_POINTS, system.feedbackBuffers[system.buffer]);
      glEndTransformFeedback();
   }
      
   // glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
   // while (result == 0)
   //    glGetQueryObjectiv(drawnQuery, GL_QUERY_RESULT_AVAILABLE, &result);
   
   // glGetQueryObjectiv(drawnQuery, GL_QUERY_RESULT, &result);
   // glDeleteQueries(1, &drawnQuery);

   // Flip buffers
#if CUBEWORLD_PLATFORM_WINDOWS
   // TODO ????
   system.buffer = uint8_t(1 - system.buffer);
#endif

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
   glDisableVertexAttribArray(3);
   CHECK_GL_ERRORS();
}

void SimpleParticleSystem::RenderParticleSystem(
   const glm::mat4& perspective,
   const glm::mat4& view,
   const glm::vec3& cameraPos,
   Engine::ParticleSystem& system
) const
{
   if (system.program == nullptr)
   {
      return;
   }

   BIND_PROGRAM_IN_SCOPE(system.program);

   system.program->UniformMatrix4f("uProjMatrix", perspective);
   system.program->UniformMatrix4f("uViewMatrix", view);
   system.program->UniformVector3f("uCameraPos", cameraPos);
   system.program->Uniform1f("uParticleLifetime", system.particleLifetime);

   for (const auto& [key, value] : system.uniforms.pairs())
   {
      if (value.IsNumber())
      {
         system.program->Uniform1f(key.GetStringValue(), value.GetFloatValue());
      }
      else if (value.IsVec3())
      {
         system.program->UniformVector3f(key.GetStringValue(), value.GetVec3());
      }
      else if (value.IsVec4())
      {
         system.program->UniformVector4f(key.GetStringValue(), value.GetVec4());
      }
      else
      {
         LOG_ERROR("Unknown shader value for %1", key.GetStringValue());
         assert(false);
      }
      CHECK_GL_ERRORS();
   }

   if (system.texture != nullptr)
   {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, system.texture->GetTexture());
      system.program->Uniform1i("uTexture", 0);
   }

   system.particleBuffers[system.buffer].Bind();

   // We enable the attribute pointers individually since a normal VBO only
   // expects to be attached to one.
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);
   glEnableVertexAttribArray(3);
   glEnableVertexAttribArray(4);

   glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Engine::ParticleSystem::Particle), 0);                   // type
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Engine::ParticleSystem::Particle), (const GLvoid*)4);    // position
   glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Engine::ParticleSystem::Particle), (const GLvoid*)16);    // rotation
   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Engine::ParticleSystem::Particle), (const GLvoid*)32);   // velocity
   glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Engine::ParticleSystem::Particle), (const GLvoid*)44);   // lifetime

   glDrawTransformFeedback(GL_POINTS, system.feedbackBuffers[system.buffer]);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
   glDisableVertexAttribArray(3);
   glDisableVertexAttribArray(4);

   CHECK_GL_ERRORS();
}

}; // namespace CubeWorld

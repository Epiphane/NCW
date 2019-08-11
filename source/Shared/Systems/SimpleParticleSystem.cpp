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
   if (!mPause)
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

         entities.Each<Transform, ParticleEmitter>([&](Engine::Entity, Transform& transform, ParticleEmitter& emitter) {
            if (!emitter.update)
            {
               return;
            }

            updater->UniformMatrix4f("uModelMatrix", transform.GetMatrix());
            updater->Uniform1f("uEmitterCooldown", emitter.emitterCooldown);
            updater->Uniform1f("uParticleLifetime", emitter.particleLifetime);

            updater->Uniform1u("uShape", (uint32_t)emitter.shape);
            updater->Uniform2f("uSpawnAge", emitter.spawnAge[0], emitter.spawnAge[1]);
            updater->UniformVector3f("uShapeParam0", emitter.shapeParam0);
            updater->Uniform1f("uShapeParam1", emitter.shapeParam1);
            updater->Uniform1f("uShapeParam2", emitter.shapeParam2);
            updater->Uniform1f("uShapeParam3", emitter.shapeParam3);

            emitter.particleBuffers[emitter.buffer].Bind();

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
            if (emitter.firstRender) {
               glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, emitter.feedbackBuffers[1]);
               glBeginTransformFeedback(GL_POINTS);
               glDrawArrays(GL_POINTS, 0, 1);
               glEndTransformFeedback();

               // [Mac] Initialize both feedback buffers by rendering initial data into them
               // TODO is this even necessary?
               emitter.particleBuffers[1 - emitter.buffer].Bind();
               glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), 0);                   // type
               glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)4);    // position
               glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)16);    // rotation
               glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)32);   // velocity
               glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)44);   // lifetime
               glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, emitter.feedbackBuffers[0]);
               glBeginTransformFeedback(GL_POINTS);
               glDrawArrays(GL_POINTS, 0, 1);
               glEndTransformFeedback();

               emitter.firstRender = false;
            }
            else {
               glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, emitter.feedbackBuffers[1 - emitter.buffer]);
               glBeginTransformFeedback(GL_POINTS);
               glDrawTransformFeedback(GL_POINTS, emitter.feedbackBuffers[emitter.buffer]);
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
            emitter.buffer = uint8_t(1 - emitter.buffer);
#endif

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glDisableVertexAttribArray(3);
            CHECK_GL_ERRORS();
         });
      }
      mUpdateClock.Elapsed();
   }

   mRenderClock.Reset();
   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   glm::vec3 position = mCamera->GetPosition();
   entities.Each<Transform, ParticleEmitter>([&](Engine::Entity /*entity*/, Transform& transform, ParticleEmitter& emitter) {
      // Render
      if (emitter.render && emitter.program != nullptr)
      {
         BIND_PROGRAM_IN_SCOPE(emitter.program);

         emitter.program->UniformMatrix4f("uProjMatrix", perspective);
         emitter.program->UniformMatrix4f("uViewMatrix", view);
         emitter.program->UniformMatrix4f("uModelMatrix", transform.GetMatrix());
         emitter.program->UniformVector3f("uCameraPos", position);
         emitter.program->Uniform1f("uParticleLifetime", emitter.particleLifetime);

         for (const auto& [key, value] : emitter.uniforms.pairs())
         {
            if (value.IsNumber())
            {
               emitter.program->Uniform1f(key.GetStringValue(), value.GetFloatValue());
            }
            else if (value.IsVec3())
            {
               emitter.program->UniformVector3f(key.GetStringValue(), value.GetVec3());
            }
            else if (value.IsVec4())
            {
               emitter.program->UniformVector4f(key.GetStringValue(), value.GetVec4());
            }
            else
            {
               LOG_ERROR("Unknown shader value for %1", key.GetStringValue());
               assert(false);
            }
            CHECK_GL_ERRORS();
         }

         if (emitter.texture != nullptr)
         {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, emitter.texture->GetTexture());
            emitter.program->Uniform1i("uTexture", 0);
         }

         emitter.particleBuffers[emitter.buffer].Bind();

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

         glDrawTransformFeedback(GL_POINTS, emitter.feedbackBuffers[emitter.buffer]);

         glDisableVertexAttribArray(0);
         glDisableVertexAttribArray(1);
         glDisableVertexAttribArray(2);
         glDisableVertexAttribArray(3);
         glDisableVertexAttribArray(4);

         CHECK_GL_ERRORS();
      }
   });
   mRenderClock.Elapsed();
}

}; // namespace CubeWorld

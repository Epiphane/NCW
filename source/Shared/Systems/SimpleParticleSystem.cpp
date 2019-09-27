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
      return FormatString("%.2fms", mUpdateClock.Average() * 1000.0);
   });
   mRenderMetric = DebugHelper::Instance().RegisterMetric("Particle Render Time", [this]() -> std::string {
      return FormatString("%.2fms", mRenderClock.Average() * 1000.0);
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
         updater->Uniform("uEmit");
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
         if (emitter.update)
         {
            UpdateParticleSystem(emitter, transform.GetMatrix(), dt);
         }
      });

      entities.Each<Transform, MultipleParticleEmitters>([&](Transform& transform, MultipleParticleEmitters& group) {
         for (MultipleParticleEmitters::Emitter* system : group.systems)
         {
            if (system != nullptr && system->update)
            {
               UpdateParticleSystem(*system, system->useEntityTransform ? transform.GetMatrix() : system->transform, dt);
            }
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
         if (system != nullptr && system->render)
         {
            RenderParticleSystem(
               perspective,
               view,
               position,
               *system
            );
         }
      }
   });
   mRenderClock.Elapsed();
}

void SimpleParticleSystem::UpdateParticleSystem(
   Engine::ParticleSystem& system,
   const glm::mat4& matrix,
   TIMEDELTA dt
) const
{
   updater->UniformMatrix4f("uModelMatrix", matrix);

   //float previousAge = system.age;
   system.age += (float)dt;

   //
   // Special case: Trails! Trails aren't updated as part of a
   // geometry shader because it's too weird and there aren't
   // enough particles to really justify pushing it to the GPU.
   //
   if (system.shape == Engine::ParticleSystem::Shape::Trail)
   {
      // For all but the two lead particles (at the end of the buffer),
      // move a small fraction towards the next trail point.
      if (system.particles.size() > 2)
      {
         for (size_t i = 0; i < system.particles.size() - 3; i += 2)
         {
            Engine::ParticleSystem::Particle& top = system.particles[i];
            Engine::ParticleSystem::Particle& bot = system.particles[i + 1];
            const Engine::ParticleSystem::Particle& nt = system.particles[i + 2];
            const Engine::ParticleSystem::Particle& nb = system.particles[i + 3];

            // Move towards the next point
            float percent = 20.0f * (float)dt;
            if (top.age >= 0.2f)
            {
               percent *= top.age * top.age;
            }

            top.pos = (1 - percent) * top.pos + percent * nt.pos;
            bot.pos = (1 - percent) * bot.pos + percent * nb.pos;

            float converge = 1 * (float)dt;
            glm::vec3 newTopPos = (1 - converge) * top.pos + converge * bot.pos;
            glm::vec3 newBotPos = (1 - converge) * bot.pos + converge * top.pos;
            top.pos = newTopPos;
            bot.pos = newBotPos;

            // Age the particles
            top.age += (float)dt;
            bot.age += (float)dt;
         }
      }

      // Prep new lead pair data
      glm::vec3 topLeadPos{matrix * glm::vec4{0, 15, 0, 1}};
      glm::vec3 botLeadPos{matrix * glm::vec4{0, -6, 0, 1}};

      glm::vec3 topStart{system.particles[system.particles.size() - 2].pos};
      glm::vec3 botStart{system.particles[system.particles.size() - 1].pos};
      if (system.firstRender)
      {
         topStart = topLeadPos;
         botStart = botLeadPos;
      }

      // Update lead pair to always follow the model.
      {
         Engine::ParticleSystem::Particle& top = system.particles[system.particles.size() - 2];
         top.pos = topLeadPos;

         Engine::ParticleSystem::Particle& bot = system.particles[system.particles.size() - 1];
         bot.pos = botLeadPos;
      }

      // Emit new lead pairs.
      system.particles[system.particles.size() - 1].age += (float)dt;
      system.particles[system.particles.size() - 2].age += (float)dt;
      if (system.launcher.lifetime == 0 || system.age < system.launcher.lifetime)
      {
         float nEmit = std::floor(system.particles[system.particles.size() - 1].age / system.launcher.cooldown);
         system.particles[system.particles.size() - 1].age -= nEmit * system.launcher.cooldown;
         system.particles[system.particles.size() - 2].age -= nEmit * system.launcher.cooldown;
         nEmit = 1;
         for (float i = 0; i < nEmit; ++i)
         {
            float percent = (i + 1) / nEmit;

            Engine::ParticleSystem::Particle newTop;
            newTop.type = 2.0f; // Particle
            newTop.pos = percent * topLeadPos + (1 - percent) * topStart;
            newTop.rot = {1,0,0,0}; // Top
            newTop.age = 0;

            Engine::ParticleSystem::Particle newBot;
            newBot.type = 2.0f; // Particle
            newBot.pos = percent * botLeadPos + (1 - percent) * botStart;
            newBot.rot = {0,1,0,0}; // Bot
            newBot.age = 0;

            system.particles.push_back(newTop);
            system.particles.push_back(newBot);
         }
      }

      system.buffers[0].data.BufferData(sizeof(Engine::ParticleSystem::Particle) * system.particles.size(), system.particles.data(), GL_STREAM_DRAW);
      system.firstRender = false;
      return;
   }

   //
   // Now update a normal particle system
   //
   if (system.launcher.lifetime == 0.0f || system.age < system.launcher.lifetime)
   {
      updater->Uniform1i("uEmit", 1);
   }
   else
   {
      updater->Uniform1i("uEmit", 0);
   }
   updater->Uniform1f("uEmitterCooldown", system.launcher.cooldown);
   updater->Uniform1f("uParticleLifetime", system.particle.lifetime);

   updater->Uniform1u("uShape", (uint32_t)system.shape);
   updater->Uniform2f("uSpawnAge", system.particle.spawnAge.min, system.particle.spawnAge.max);
   if (system.shape == Engine::ParticleSystem::Shape::Cone)
   {
      updater->UniformVector3f("uShapeParam0", system.shapeConfig.cone.direction);
      updater->Uniform1f("uShapeParam1", system.shapeConfig.cone.radius);
      updater->Uniform1f("uShapeParam2", system.shapeConfig.cone.height.min);
      updater->Uniform1f("uShapeParam3", system.shapeConfig.cone.height.max);
   }

   system.buffers[system.buffer].Bind();

   // We enable the attribute pointers individually since a normal VBO only
   // expects to be attached to one.
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);
   glEnableVertexAttribArray(3);
   glEnableVertexAttribArray(4);

   glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), 0);                   // type
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)4);    // position
   glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)16);   // rotation
   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)32);   // velocity
   glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)44);   // lifetime

#if CUBEWORLD_DIAGNOSE_PARTICLE_OUTPUT
   GLuint drawnQuery;
   int result = 0;
   glGenQueries(1, &drawnQuery);
   glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, drawnQuery);
#endif

   // Begin rendering
   if (system.firstRender) {
      system.buffers[1].Begin(GL_POINTS);
      glDrawArrays(GL_POINTS, 0, 1);
      system.buffers[0].End();

      // [Mac] Initialize both feedback buffers by rendering initial data into them
      // TODO is this even necessary?
      system.buffers[1 - system.buffer].Bind();
      glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), 0);                   // type
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)4);    // position
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)16);    // rotation
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)32);   // velocity
      glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)44);   // lifetime
      system.buffers[0].Begin(GL_POINTS);
      glDrawArrays(GL_POINTS, 0, 1);
      system.buffers[0].End();

      system.firstRender = false;
   }
   else {
      system.buffers[1 - system.buffer].Begin(GL_POINTS);
      system.buffers[system.buffer].Draw(GL_POINTS);
      system.buffers[1 - system.buffer].End();
   }

#if CUBEWORLD_DIAGNOSE_PARTICLE_OUTPUT
   glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
   while (result == 0)
      glGetQueryObjectiv(drawnQuery, GL_QUERY_RESULT_AVAILABLE, &result);

   glGetQueryObjectiv(drawnQuery, GL_QUERY_RESULT, &result);
   glDeleteQueries(1, &drawnQuery);
#endif

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

   if (system.shape == Engine::ParticleSystem::Shape::Trail && system.particles.size() < 3)
   {
      return;
   }

   BIND_PROGRAM_IN_SCOPE(system.program);

   system.program->UniformMatrix4f("uProjMatrix", perspective);
   system.program->UniformMatrix4f("uViewMatrix", view);
   system.program->UniformVector3f("uCameraPos", cameraPos);
   system.program->Uniform1f("uParticleLifetime", system.particle.lifetime);

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
         LOG_ERROR("Unknown shader value for {key}", key.GetStringValue());
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

   system.buffers[system.buffer].Bind();

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

   if (system.shape == Engine::ParticleSystem::Shape::Trail)
   {
      glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)system.particles.size());
   }
   else
   {
      system.buffers[system.buffer].Draw(GL_POINTS);
   }

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
   glDisableVertexAttribArray(3);
   glDisableVertexAttribArray(4);

   CHECK_GL_ERRORS();
}

}; // namespace CubeWorld

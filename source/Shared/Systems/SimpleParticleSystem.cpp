// By Thomas Steinke

#include <glad/glad.h>
#include <glm/ext.hpp>

#include <RGBDesignPatterns/Scope.h>
#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>

#include "SimpleParticleSystem.h"

namespace CubeWorld
{

// ------------------------------------------------------------------------------------------------
// |                                                                                              |
// |                                     ParticleEmitter                                          |
// |                                                                                              |
// ------------------------------------------------------------------------------------------------
std::unordered_map<std::string, std::unique_ptr<Engine::Graphics::Program>> ParticleEmitter::programs;

ParticleEmitter::ParticleEmitter()
   : name("")
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

void ParticleEmitter::Initialize(const Options& options)
{
   name = options.name;

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
         maybeProgram.Failure().WithContext("Failed loading particle shader").Log();
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

   // Initialize emitter particle
   std::vector<Particle> data;
   data.resize(options.maxParticles);
   data[0].type = 0; // Emitter
   data[0].pos = {0, 0, 0};
   data[0].vel = {0, 0, 0};
   data[0].age = 0;

   particleBuffers[0].BufferData(sizeof(Particle) * data.size(), data.data(), GL_STATIC_DRAW);
   particleBuffers[1].BufferData(sizeof(Particle) * data.size(), data.data(), GL_STATIC_DRAW);
}

ParticleEmitter::ParticleEmitter(const Options& options) : ParticleEmitter()
{
   Initialize(options);
}

ParticleEmitter::ParticleEmitter(const std::string& dir, const BindingProperty& serialized) : ParticleEmitter()
{
   Options options;
   options.name = serialized["name"];

   options.vertexShader = Paths::Join(dir, options.name + ".vert");
   options.geometryShader = Paths::Join(dir, options.name + ".geom");
   options.fragmentShader = Paths::Join(dir, options.name + ".frag");

   Initialize(options);

   launcherLifetime = serialized["launcher"]["lifetime"].GetDoubleValue();
   particleLifetime = serialized["particle"]["lifetime"].GetDoubleValue();
}

ParticleEmitter::ParticleEmitter(const ParticleEmitter& other) : ParticleEmitter()
{
   program = other.program;
}

ParticleEmitter::~ParticleEmitter()
{
   glDeleteTransformFeedbacks(2, feedbackBuffers);
}

BindingProperty ParticleEmitter::Serialize()
{
   BindingProperty result;

   result["name"] = name;
   result["launcher"]["lifetime"] = launcherLifetime;
   result["particle"]["lifetime"] = particleLifetime;

   return result;
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
      glEnable(GL_RASTERIZER_DISCARD);
      CUBEWORLD_SCOPE_EXIT([&] { glDisable(GL_RASTERIZER_DISCARD); });
      entities.Each<ParticleEmitter>([&](Engine::Entity, ParticleEmitter& emitter) {
         if (mRandom)
         {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_1D, mRandom->GetTexture());
            updater->Uniform1i("uRandomTexture", 3);
         }

         updater->Uniform1f("uLauncherLifetime", (float)emitter.launcherLifetime);
         updater->Uniform1f("uShellLifetime", (float)emitter.particleLifetime);
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

   glFlush();

   mRenderClock.Reset();
   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   glm::vec3 position = mCamera->GetPosition();
   entities.Each<ParticleEmitter>([&](Engine::Entity /*entity*/, ParticleEmitter& emitter) {
      // Render
      if (emitter.program != nullptr)
      {
         BIND_PROGRAM_IN_SCOPE(emitter.program);

         emitter.program->UniformMatrix4f("uProjMatrix", perspective);
         emitter.program->UniformMatrix4f("uViewMatrix", view);
         emitter.program->UniformVector3f("uCameraPos", position);
         emitter.program->Uniform1f("uBillboardSize", 10.0f);

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
         glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleEmitter::Particle), (const GLvoid*)28);   // age

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

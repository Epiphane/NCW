// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include <Shared/Helpers/Asset.h>
#include "SimpleAnimationSystem.h"
#include "AnimationDebugSystem.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

std::unique_ptr<Engine::Graphics::Program> AnimationDebugSystem::program;

void AnimationDebugSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load(
          Asset::Shader("PhysicsDebug.vert"),
          Asset::Shader("PhysicsDebug.geom"),
          Asset::Shader("PhysicsDebug.frag")
      );
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading Physics Debug shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);
      program->Uniform("uProjMatrix");
      program->Uniform("uViewMatrix");
      program->Uniform("uModelMatrix");
      program->Uniform("uPosition");
      program->Uniform("uSize");
   }
}

void AnimationDebugSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   if (!mActive)
   {
      return;
   }

   BIND_PROGRAM_IN_SCOPE(program);

   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   program->UniformMatrix4f("uProjMatrix", perspective);
   program->UniformMatrix4f("uViewMatrix", view);

   entities.Each<Engine::Transform, SimpleAnimationController>([&](Engine::Entity, Engine::Transform& transform, SimpleAnimationController& controller) {
      for (const auto& event : controller.states[controller.current].events)
      {
         if (controller.time >= event.start && controller.time <= event.end)
         {
            switch (event.type)
            {
            case SkeletonAnimations::Event::Type::Strike:
            {
               for (const auto& s : controller.skeletons)
               {
                  if (s->boneLookup.count(event.strike.bone) != 0)
                  {
                     glm::mat4 matrix = transform.GetMatrix() * s->bones[s->boneLookup.at(event.strike.bone)].matrix;
                     program->UniformMatrix4f("uModelMatrix", matrix);

                     glm::vec4 pos = glm::vec4(event.strike.offset, 1.0f);
                     program->UniformVector3f("uPosition", event.strike.offset);
                     program->UniformVector3f("uSize", event.strike.size);

                     glDrawArrays(GL_POINTS, 0, 1);
                     CHECK_GL_ERRORS();
                     break;
                  }
               }

               break;
            }

            default:
               // Nothing to do
               break;
            }
         }
      }
   });
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

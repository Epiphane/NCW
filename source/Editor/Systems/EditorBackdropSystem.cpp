// By Thomas Steinke

#include <cassert>
#include <RGBLogger/Logger.h>
#include <RGBText/StringHelper.h>

#include "EditorBackdropSystem.h"

namespace CubeWorld
{

namespace Editor
{

std::unique_ptr<Engine::Graphics::Program> EditorBackdropSystem::program;

Maybe<void> AddFloor(Engine::EntityManager &entities, glm::vec3 color)
{
   std::vector<Voxel::Data> carpet;

   const glm::vec3 BASE(18, 18, 18);
   const int size = 20;
   for (int i = -size; i <= size; ++i) {
      for (int j = -size; j <= size; ++j) {
         double x = (double)i / (2 * size_t(size));
         double y = (double)j / (2 * size_t(size));
         double expectedX = 1.0 - 4.0 * std::pow(y - 0.5, 2);
         double dist = 5.0 * std::abs(x - expectedX);
         dist += (rand() % 500) / 1000.0 - 0.25;
         // Curve it from (0, 1)
         dist = 1.0 / (1 + std::pow(2, dist));

         glm::vec4 blended{
            std::floor((1 - dist) * BASE.r + dist * color.r),
            std::floor((1 - dist) * BASE.g + dist * color.g),
            std::floor((1 - dist) * BASE.b + dist * color.b),
            1
         };

         glm::vec3 position = glm::vec3(i, -0.5f, j);
         carpet.push_back(Voxel::Data(position, blended, Voxel::Top));
      }
   }

   assert(carpet.size() > 0);

   Engine::Entity entity = entities.Create(0, -0.5f, 0);
   entity.Add<VoxelRender>(std::move(carpet));
   return Success;
}

// ------------------------------------------------------------------------------------------------
// |                                                                                              |
// |                                  EditorBackdropSystem                                        |
// |                                                                                              |
// ------------------------------------------------------------------------------------------------
EditorBackdropSystem::EditorBackdropSystem(Engine::Graphics::Camera* camera)
   : mCamera(camera)
{
   std::vector<glm::vec3> data{{0, 0, 0}};
   mVBO.BufferData(data.size(), data.data(), GL_STATIC_DRAW);
}

EditorBackdropSystem::~EditorBackdropSystem()
{}

void EditorBackdropSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load(
         "Shaders/EditorBackdrop.vert",
         "Shaders/EditorBackdrop.geom",
         "Shaders/EditorBackdrop.frag"
      );
      if (!maybeProgram)
      {
         maybeProgram.Failure().WithContext("Failed loading EditorBackdrop shader").Log();
      }
      else
      {
         program = std::move(*maybeProgram);
         program->Attrib("aPosition");
         program->Uniform("uProjMatrix");
         program->Uniform("uViewMatrix");
         program->Uniform("uModelMatrix");
         program->Uniform("uColor");
      }
   }
}

void EditorBackdropSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   BIND_PROGRAM_IN_SCOPE(program);

   program->UniformMatrix4f("uProjMatrix", mCamera->GetPerspective());
   program->UniformMatrix4f("uViewMatrix", mCamera->GetView());
   
   entities.Each<Engine::Transform, EditorWireframe>([&](Engine::Entity, Engine::Transform& transform, EditorWireframe& backdrop) {
      program->UniformMatrix4f("uModelMatrix", transform.GetMatrix());
      program->UniformVector3f("uColor", backdrop.color);

      mVBO.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)0);

      glDrawArrays(GL_POINTS, 0, 1);
      CHECK_GL_ERRORS();
   });
}

}; // namespace Editor

}; // namespace CubeWorld

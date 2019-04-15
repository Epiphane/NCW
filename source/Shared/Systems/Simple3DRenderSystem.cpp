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

#include "Simple3DRenderSystem.h"

namespace CubeWorld
{

Simple3DRender::Simple3DRender(std::vector<GLfloat>&& points, std::vector<GLfloat>&& colors)
   : mVertices(Engine::Graphics::VBO::Vertices)
   , mColors(Engine::Graphics::VBO::Colors)
   , mCount(int(points.size()))
{
   mVertices.BufferData(sizeof(GLfloat) * mCount, &points[0], GL_STATIC_DRAW);
   mColors.BufferData(sizeof(GLfloat) * mCount, &colors[0], GL_STATIC_DRAW);
}

Simple3DRender::Simple3DRender(const Simple3DRender& other)
   : mVertices(other.mVertices)
   , mColors(other.mColors)
{}

std::unique_ptr<Engine::Graphics::Program> Simple3DRenderSystem::program = nullptr;

Simple3DRenderSystem::Simple3DRenderSystem(Engine::Graphics::Camera* camera) : mCamera(camera)
{
}

Simple3DRenderSystem::~Simple3DRenderSystem()
{
}

void Simple3DRenderSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/Stupid.vert", "Shaders/Stupid.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading Stupid shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);
      program->Attrib("aPosition");
      program->Attrib("aColor");
      program->Uniform("uProjMatrix");
      program->Uniform("uViewMatrix");
      program->Uniform("uModelMatrix");
   }

   metric = DebugHelper::Instance()->RegisterMetric("3D Render Time", [this]() -> std::string {
      return Format::FormatString("%.2fms", mClock.Average() * 1000.0);
   });
}

using Transform = Engine::Transform;

void Simple3DRenderSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   BIND_PROGRAM_IN_SCOPE(program);

   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   program->UniformMatrix4f("uProjMatrix", perspective);
   program->UniformMatrix4f("uViewMatrix", view);

   mClock.Reset();
   entities.Each<Transform, Simple3DRender>([&](Engine::Entity /*entity*/, Transform& transform, Simple3DRender& render) {
      render.mVertices.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
      render.mColors.AttribPointer(program->Attrib("aColor"), 3, GL_FLOAT, GL_FALSE, 0, 0);

      glm::mat4 model = transform.GetMatrix();
      program->UniformMatrix4f("uModelMatrix", model);
      
      glDrawArrays(GL_TRIANGLES, 0, render.mCount);

      CHECK_GL_ERRORS();
   });
   mClock.Elapsed();
}

}; // namespace CubeWorld

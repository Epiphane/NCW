// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Timer.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include <Game/DebugHelper.h>
#include "Simple3DRenderSystem.h"

namespace CubeWorld
{

namespace Game
{
   
Simple3DRender::Simple3DRender(std::vector<GLfloat>&& points, std::vector<GLfloat>&& colors)
   : mVertices(Engine::Graphics::Vertices)
   , mColors(Engine::Graphics::Colors)
   , mCount(int(points.size()))
{
   mVertices.BufferData(sizeof(GLfloat) * mCount, &points[0], GL_STATIC_DRAW);
   mColors.BufferData(sizeof(GLfloat) * mCount, &colors[0], GL_STATIC_DRAW);
}

Simple3DRender::Simple3DRender(const Simple3DRender& other)
   : mVertices(other.mVertices)
   , mColors(other.mColors)
{}

REGISTER_GLUINT(Simple3DRenderSystem, program)
REGISTER_GLUINT(Simple3DRenderSystem, aPosition)
REGISTER_GLUINT(Simple3DRenderSystem, aColor)
REGISTER_GLUINT(Simple3DRenderSystem, uProjMatrix)
REGISTER_GLUINT(Simple3DRenderSystem, uViewMatrix)
REGISTER_GLUINT(Simple3DRenderSystem, uModelMatrix)

Simple3DRenderSystem::Simple3DRenderSystem(Engine::Graphics::Camera* camera) : mCamera(camera)
{
}

Simple3DRenderSystem::~Simple3DRenderSystem()
{
}

void Simple3DRenderSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
   if (program != 0)
   {
      return;
   }

   program = Engine::Graphics::LoadProgram("Shaders/Stupid.vert", "Shaders/Stupid.frag");

   if (program == 0)
   {
      LOG_ERROR("Could not load Stupid shader");
      return;
   }

   DISCOVER_ATTRIBUTE(aPosition);
   DISCOVER_ATTRIBUTE(aColor);
   DISCOVER_UNIFORM(uProjMatrix);
   DISCOVER_UNIFORM(uViewMatrix);
   DISCOVER_UNIFORM(uModelMatrix);

   Game::DebugHelper::Instance()->RegisterMetric("3D Render Time", [this]() -> std::string {
      return Format::FormatString("%1ms", std::round(mClock.Average() * 100000.0) / 100);
   });
}

using Transform = Engine::Transform;

void Simple3DRenderSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   glUseProgram(program);

   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();

   glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, glm::value_ptr(perspective));
   glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, glm::value_ptr(view));

   mClock.Reset();
   entities.Each<Transform, Simple3DRender>([&](Engine::Entity /*entity*/, Transform& transform, Simple3DRender& render) {
      render.mVertices.AttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
      render.mColors.AttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

      glm::mat4 model = transform.GetMatrix();
      glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));
      
      glDrawArrays(GL_TRIANGLES, 0, render.mCount);
   });
   mClock.Elapsed();

   // Cleanup.
   glDisableVertexAttribArray(aPosition);
   glDisableVertexAttribArray(aColor);
   glUseProgram(0);
}

}; // namespace Game

}; // namespace CubeWorld

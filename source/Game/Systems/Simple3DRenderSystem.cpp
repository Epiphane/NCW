// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Logger/Logger.h>
#include <Engine/Core/Scope.h>
#include <Engine/Graphics/Program.h>

#include "Simple3DRenderSystem.h"

namespace CubeWorld
{

namespace Game
{

#define REGISTER_GLUINT(RENDERER, value) GLuint RENDERER::value = 0;
#define DISCOVER_UNIFORM(value) {value = glGetUniformLocation(program, #value); GLenum error = glGetError(); assert(error == 0); }
#define DISCOVER_ATTRIBUTE(value) {value = glGetAttribLocation(program, #value); GLenum error = glGetError(); assert(error == 0); }

REGISTER_GLUINT(Simple3DRenderSystem, program)
REGISTER_GLUINT(Simple3DRenderSystem, aPosition)
REGISTER_GLUINT(Simple3DRenderSystem, aColor)
REGISTER_GLUINT(Simple3DRenderSystem, uProjMatrix)
REGISTER_GLUINT(Simple3DRenderSystem, uViewMatrix)
REGISTER_GLUINT(Simple3DRenderSystem, uModelMatrix)

Simple3DRenderSystem::Simple3DRenderSystem(Engine::Graphics::Camera* camera) : mCamera(camera)
{
   using namespace Engine::Graphics;

   mVertices = std::make_unique<VBO>(Vertices);
   mColors = std::make_unique<VBO>(Colors);

   static const GLfloat points[] = {
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      0.0f,  1.0f, 0.0f,
   };

   static const GLfloat colors[] = {
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      0.0f,  1.0f, 0.0f,
   };

   mVertices->BufferData(sizeof(points), (void *)points, GL_STATIC_DRAW);
   mColors->BufferData(sizeof(colors), (void *)colors, GL_STATIC_DRAW);
}

Simple3DRenderSystem::~Simple3DRenderSystem()
{
}

void Simple3DRenderSystem::Configure(Engine::EntityManager& /*entities*/)
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
}

using Transform = Engine::Transform;

void Simple3DRenderSystem::Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA)
{
   glUseProgram(program);

   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();

   glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, glm::value_ptr(perspective));
   glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, glm::value_ptr(view));

   mVertices->AttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mColors->AttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

   entities.Each<Transform, Simple3DRender>([&](Engine::Entity /*entity*/, Transform& transform, Simple3DRender& /*render*/) {
      glm::mat4 model = glm::translate(glm::mat4(1), transform.position);
      glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));

      glDrawArrays(GL_TRIANGLES, 0, 3);
   });

   // Cleanup.
   glDisableVertexAttribArray(aPosition);
   glDisableVertexAttribArray(aColor);
   glUseProgram(0);
}

}; // namespace Game

}; // namespace CubeWorld
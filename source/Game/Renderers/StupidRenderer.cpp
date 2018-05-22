// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Logger/Logger.h>
#include <Engine/GameObject/GameObject.h>
#include <Engine/Core/Scope.h>
#include <Engine/Graphics/Program.h>

#include "StupidRenderer.h"

namespace CubeWorld
{

namespace Game
{

#define REGISTER_GLUINT(RENDERER, value) GLuint RENDERER::value = 0;
#define DISCOVER_UNIFORM(value) {value = glGetUniformLocation(program, #value); GLenum error = glGetError(); assert(error == 0); }
#define DISCOVER_ATTRIBUTE(value) {value = glGetAttribLocation(program, #value); GLenum error = glGetError(); assert(error == 0); }

REGISTER_GLUINT(StupidRenderer, program)
REGISTER_GLUINT(StupidRenderer, aPosition)
REGISTER_GLUINT(StupidRenderer, aColor)
REGISTER_GLUINT(StupidRenderer, uProjMatrix)
REGISTER_GLUINT(StupidRenderer, uViewMatrix)
REGISTER_GLUINT(StupidRenderer, uModelMatrix)

StupidRenderer::StupidRenderer()
{
   StupidRenderer::Initalize();

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

StupidRenderer::~StupidRenderer()
{
}

void StupidRenderer::Initalize()
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

void StupidRenderer::Render(glm::mat4 perspective, glm::mat4 view, glm::mat4 model)
{
   glUseProgram(program);

   glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, glm::value_ptr(perspective));
   glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, glm::value_ptr(view));
   glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));

   mVertices->AttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mColors->AttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glDrawArrays(GL_TRIANGLES, 0, 3);

   // Cleanup.
   glDisableVertexAttribArray(aPosition);
   glDisableVertexAttribArray(aColor);
   glUseProgram(0);
}

std::unique_ptr<Engine::Graphics::Renderer> StupidRenderer::Clone()
{
   return std::make_unique<StupidRenderer>();
}

}; // namespace Game

}; // namespace CubeWorld
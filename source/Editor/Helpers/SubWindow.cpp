// By Thomas Steinke

#include <cassert>

#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include "SubWindow.h"

namespace CubeWorld
{

namespace Editor
{

REGISTER_GLUINT(SubWindow, program);
REGISTER_GLUINT(SubWindow, aPosition);
REGISTER_GLUINT(SubWindow, aUV);
REGISTER_GLUINT(SubWindow, uTexture);

SubWindow::SubWindow(
   Bounded& parent,
   const Options& options)
   : mOptions(options)
   , mParent(parent)
   , mFramebuffer(parent.Width() * options.w, parent.Height() * options.h)
   , mVBO(Engine::Graphics::VBO::DataType::Vertices)
{
   if (program == 0)
   {
      program = Engine::Graphics::LoadProgram("Shaders/PassthroughTexture.vert", "Shaders/PassthroughTexture.frag");

      if (program == 0)
      {
         LOG_ERROR("Could not load PassthroughTexture shader");
         return;
      }

      DISCOVER_ATTRIBUTE(aPosition);
      DISCOVER_ATTRIBUTE(aUV);
      DISCOVER_UNIFORM(uTexture);
   }

   float x = 2.0f * mOptions.x - 1.0f;
   float y = 2.0f * mOptions.y - 1.0f;
   float w = 2.0f * mOptions.w;
   float h = 2.0f * mOptions.h;

   std::vector<GLfloat> vboData = {
      x,     y,     0.0f, 0.0f, 0.0f,
      x + w, y,     0.0f, 1.0f, 0.0f,
      x,     y + h, 0.0f, 0.0f, 1.0f,
      x,     y + h, 0.0f, 0.0f, 1.0f,
      x + w, y,     0.0f, 1.0f, 0.0f,
      x + w, y + h, 0.0f, 1.0f, 1.0f,
   };
   mVBO.BufferData(sizeof(GLfloat) * vboData.size(), &vboData[0], GL_STATIC_DRAW);
}

SubWindow::~SubWindow()
{
}

void SubWindow::Bind()
{
   mFramebuffer.Bind();
}

void SubWindow::Unbind()
{
   mFramebuffer.Unbind();
}

void SubWindow::Render()
{
   glUseProgram(program);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mFramebuffer.GetTexture());
   glUniform1i(uTexture, 0);

   mVBO.AttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
   mVBO.AttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);
   GLenum err = glGetError();
   assert(err == 0);

   // Cleanup.
   glDisableVertexAttribArray(aPosition);
   glDisableVertexAttribArray(aUV);

   glUseProgram(0);
}

}; // namespace Editor

}; // namespace CubeWorld

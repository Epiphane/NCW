// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include "StateWindow.h"

namespace CubeWorld
{

namespace Editor
{

std::unique_ptr<Engine::Graphics::Program> StateWindow::program = nullptr;

StateWindow::StateWindow(
   Bounded& parent,
   const Options& options)
   : Element(parent, options)
   , mFramebuffer(GLsizei(parent.GetWidth() * options.w), GLsizei(parent.GetHeight() * options.h))
   , mVBO(Engine::Graphics::VBO::DataType::Vertices)
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/PassthroughTexture.vert", "Shaders/PassthroughTexture.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading PassthroughTexture shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);

      program->Attrib("aPosition");
      program->Attrib("aUV");
      program->Uniform("uTexture");
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
   mVBO.BufferData(GLsizei(sizeof(GLfloat) * vboData.size()), &vboData[0], GL_STATIC_DRAW);
}

StateWindow::~StateWindow()
{
}

void StateWindow::Update(TIMEDELTA dt)
{
   // Draw elements
   mFramebuffer.Bind();
   Engine::StateManager::Instance()->Update(dt);
   mFramebuffer.Unbind();

   // Draw to the screen
   BIND_PROGRAM_IN_SCOPE(program);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mFramebuffer.GetTexture());
   program->Uniform1i("uTexture", 0);

   mVBO.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
   mVBO.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);
}

void StateWindow::MouseDown(int button, double x, double y)
{
   double localX = (x - mOptions.x) / mOptions.w;
   double localY = (y - mOptions.y) / mOptions.h;
   Engine::StateManager::Instance()->Emit<MouseDownEvent>(button, x, y);
}

void StateWindow::MouseUp(int button, double x, double y)
{
   double localX = (x - mOptions.x) / mOptions.w;
   double localY = (y - mOptions.y) / mOptions.h;
   Engine::StateManager::Instance()->Emit<MouseUpEvent>(button, x, y);
}

void StateWindow::MouseClick(int button, double x, double y)
{
   double localX = (x - mOptions.x) / mOptions.w;
   double localY = (y - mOptions.y) / mOptions.h;
   Engine::StateManager::Instance()->Emit<MouseClickEvent>(button, x, y);
}

}; // namespace Editor

}; // namespace CubeWorld

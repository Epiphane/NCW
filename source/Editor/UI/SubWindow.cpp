// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include "SubWindow.h"

namespace CubeWorld
{

namespace Editor
{

std::unique_ptr<Engine::Graphics::Program> SubWindow::program = nullptr;

SubWindow::SubWindow(
   Bounded& parent,
   const Options& options)
   : Element(parent, options)
   , mChildren{}
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

SubWindow::~SubWindow()
{
}

void SubWindow::Update(TIMEDELTA dt)
{
   if (!mActive)
   {
      return;
   }

   // Draw elements
   mFramebuffer.Bind();
   for (std::unique_ptr<Element> &child : mChildren)
   {
      if (child->IsActive()) {
         child->Update(dt);
      }
   }
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

void SubWindow::MouseDown(int button, double x, double y)
{
   if (!mActive)
   {
      return;
   }

   double localX = (x - mOptions.x) / mOptions.w;
   double localY = (y - mOptions.y) / mOptions.h;

   for (std::unique_ptr<Element> &child : mChildren)
   {
      if (child->IsActive()) {
         child->MouseDown(button, localX, localY);
      }
   }
}

void SubWindow::MouseUp(int button, double x, double y)
{
   if (!mActive)
   {
      return;
   }

   double localX = (x - mOptions.x) / mOptions.w;
   double localY = (y - mOptions.y) / mOptions.h;

   for (std::unique_ptr<Element> &child : mChildren)
   {
      if (child->IsActive()) {
         child->MouseUp(button, localX, localY);
      }
   }
}

void SubWindow::MouseClick(int button, double x, double y)
{
   if (!mActive)
   {
      return;
   }

   double localX = (x - mOptions.x) / mOptions.w;
   double localY = (y - mOptions.y) / mOptions.h;

   for (std::unique_ptr<Element> &child : mChildren)
   {
      if (child->IsActive()) {
         child->MouseClick(button, localX, localY);
      }
   }
}

void SubWindow::MouseDrag(int button, double x, double y)
{
   if (!mActive)
   {
      return;
   }

   double localX = (x - mOptions.x) / mOptions.w;
   double localY = (y - mOptions.y) / mOptions.h;

   for (std::unique_ptr<Element> &child : mChildren)
   {
      if (child->IsActive()) {
         child->MouseDrag(button, localX, localY);
      }
   }
}

void SubWindow::MouseMove(double x, double y)
{
   if (!mActive)
   {
      return;
   }

   // TODO: Consider not sending events if they don't occur within this SubWindow.
   // The problem: if something reacts to being moused over (e.g. a button highlighting)
   // it needs to know if the mouse has left, so it can update to reflect that.
   //if (!ContainsPoint(x, y))
   //{
   //   return;
   //}

   double localX = (x - mOptions.x) / mOptions.w;
   double localY = (y - mOptions.y) / mOptions.h;

   for (std::unique_ptr<Element> &child : mChildren)
   {
      if (child->IsActive()) {
         child->MouseMove(localX, localY);
      }
   }
}

Element* SubWindow::AddChild(std::unique_ptr<Element>&& element)
{
   Element* elem = element.get();
   mChildren.push_back(std::move(element));
   return elem;
}

}; // namespace Editor

}; // namespace CubeWorld

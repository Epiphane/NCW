// By Thomas Steinke

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>

#include "Rect.h"

namespace CubeWorld
{

namespace Aggregator
{

std::unique_ptr<Engine::Graphics::Program> Rect::program = nullptr;

Rect::Rect()
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/DebugRect.vert", "Shaders/DebugRect.geom", "Shaders/DebugRect.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading DebugRect shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);
      program->Attrib("aPosition");
      program->Uniform("uWindowSize");
   }
}

void Rect::Render()
{
   Engine::Window* pWindow = Engine::Window::Instance();

   BIND_PROGRAM_IN_SCOPE(program);

   program->Uniform2f("uWindowSize", static_cast<GLfloat>(pWindow->GetWidth()), static_cast<GLfloat>(pWindow->GetHeight()));

   mVBO.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(RectData), (void*)0);
   mVBO.AttribPointer(program->Attrib("aColor"), 4, GL_FLOAT, GL_FALSE, sizeof(RectData), (void*)(sizeof(glm::vec3)));

   glDrawArrays(GL_LINES, 0, GLsizei(mData.size()));
}

}; // namespace Aggregator

}; // namespace CubeWorld
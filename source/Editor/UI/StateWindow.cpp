// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "StateWindow.h"

namespace CubeWorld
{

namespace Editor
{

std::unique_ptr<Engine::Graphics::Program> StateWindow::program = nullptr;

StateWindow::StateWindow(Engine::UIRoot* root, UIElement* parent, std::unique_ptr<Engine::State>&& state)
   : UIElement(root, parent)
   , mState(nullptr)
   , mFramebuffer(Engine::Window::Instance()->GetWidth(), Engine::Window::Instance()->GetHeight())
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/DebugText.vert", "Shaders/DebugText.geom", "Shaders/DebugText.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading DebugText shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);
      program->Attrib("aPosition");
      program->Uniform("uTexture");
      program->Uniform("uWindowSize");
   }

   if (state)
   {
      SetState(std::move(state));
   }
}

void StateWindow::SetState(std::unique_ptr<Engine::State>&& state)
{
   mState = std::move(state);
   mState->EnsureLoaded();
}

void StateWindow::AddVertices(std::vector<Engine::Graphics::Font::CharacterVertexUV>& outVertices)
{
   Engine::Graphics::Font::CharacterVertexUV bottomLeft, topRight;
   bottomLeft.position = glm::vec2(mFrame.left.int_value(), mFrame.bottom.int_value());
   topRight.position = glm::vec2(mFrame.right.int_value(), mFrame.top.int_value());

   bottomLeft.uv = glm::vec2(0, 0);
   topRight.uv = glm::vec2(1, 1);

   outVertices.push_back(bottomLeft);
   outVertices.push_back(topRight);

   UIElement::AddVertices(outVertices);
}

void StateWindow::Update(TIMEDELTA dt)
{
   // Draw elements
   mFramebuffer.Bind();
   mState->Update(dt);
   mFramebuffer.Unbind();
}

size_t StateWindow::Render(Engine::Graphics::VBO& vbo, size_t offset)
{
   Engine::Window* pWindow = Engine::Window::Instance();

   {
      BIND_PROGRAM_IN_SCOPE(program);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mFramebuffer.GetTexture());
      program->Uniform1i("uTexture", 0);
      program->Uniform2f("uWindowSize", static_cast<GLfloat>(pWindow->GetWidth()), static_cast<GLfloat>(pWindow->GetHeight()));

      vbo.AttribPointer(program->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)offset);
      vbo.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(glm::vec2) + offset));

      glDrawArrays(GL_LINES, 0, 2);
   }

   return UIElement::Render(vbo, offset + sizeof(Engine::Graphics::Font::CharacterVertexUV) * 2);
}

}; // namespace Editor

}; // namespace CubeWorld

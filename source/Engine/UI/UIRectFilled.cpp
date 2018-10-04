// By Thomas Steinke

#include <glm/glm.hpp>

#include <Engine/Core/Window.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include "UIRectFilled.h"

namespace CubeWorld
{

namespace Engine
{

std::unique_ptr<Engine::Graphics::Program> UIRectFilled::program = nullptr;

UIRectFilled::UIRectFilled(UIRoot* root, UIElement* parent) : UIElement(root, parent)
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
      program->Uniform("uColor");
      program->Uniform("uWindowSize");
   }
}

void UIRectFilled::AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices)
{
   Graphics::Font::CharacterVertexUV topLeft, bottomRight;
   topLeft    .position = glm::vec2(mFrame.left.int_value(),  mFrame.top.int_value());
   bottomRight.position = glm::vec2(mFrame.right.int_value(), mFrame.bottom.int_value());

   topLeft.uv = glm::vec2(0, 0);
   bottomRight.uv = glm::vec2(0, 0);

   outVertices.push_back(topLeft);
   outVertices.push_back(bottomRight);

   UIElement::AddVertices(outVertices);
}

size_t UIRectFilled::Render(Engine::Graphics::VBO& vbo, size_t offset)
{
   Window* pWindow = Window::Instance();

   BIND_PROGRAM_IN_SCOPE(program);

   program->Uniform4f("uColor", mColor.r, mColor.g, mColor.b, mColor.a);
   program->Uniform2f("uWindowSize", static_cast<GLfloat>(pWindow->GetWidth()), static_cast<GLfloat>(pWindow->GetHeight()));

   vbo.AttribPointer(program->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Graphics::Font::CharacterVertexUV), (void*)offset);

   glDrawArrays(GL_LINES, 0, 2);

   // Cleanup.
   glDisableVertexAttribArray(program->Attrib("aPosition"));

   offset = UIElement::Render(vbo, offset + sizeof(Graphics::Font::CharacterVertexUV) * 2);

   return offset;
}

}; // namespace Engine

}; // namespace CubeWorld

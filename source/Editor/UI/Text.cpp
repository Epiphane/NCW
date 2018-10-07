// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Shared/Helpers/Asset.h>

#include "Text.h"

namespace CubeWorld
{

namespace Editor
{

std::unique_ptr<Engine::Graphics::Program> Text::program = nullptr;

Text::Text(Engine::UIRoot* root, UIElement* parent, const Options& options)
   : UIElement(root, parent)
   , mText("")
   , mTextToRender("")
{
   auto maybeFont = Engine::Graphics::FontManager::Instance()->GetFont(Asset::Font(options.font));
   assert(maybeFont);
   mFont = *maybeFont;

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
      program->Attrib("aUV");
      program->Uniform("uTexture");
      program->Uniform("uWindowSize");
   }

   SetText(options.text);
}

void Text::AddVertices(std::vector<Engine::Graphics::Font::CharacterVertexUV>& outVertices)
{
   std::vector<Engine::Graphics::Font::CharacterVertexUV> uvs = mFont->Write(mFrame.left.int_value(), mFrame.bottom.int_value(), 1, mTextToRender);

   outVertices.insert(outVertices.end(), uvs.begin(), uvs.end());

   UIElement::AddVertices(outVertices);
}

size_t Text::Render(Engine::Graphics::VBO& vbo, size_t offset)
{
   if (mTextToRender.empty())
   {
      return offset;
   }

   Engine::Window* pWindow = Engine::Window::Instance();

   {
      BIND_PROGRAM_IN_SCOPE(program);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mFont->GetTexture());
      program->Uniform1i("uTexture", 0);
      program->Uniform2f("uWindowSize", static_cast<GLfloat>(pWindow->GetWidth()), static_cast<GLfloat>(pWindow->GetHeight()));

      vbo.AttribPointer(program->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)offset);
      vbo.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2 + offset));

      glDrawArrays(GL_LINES, 0, 2 * mTextToRender.size());
   }

   return UIElement::Render(vbo, offset + sizeof(Engine::Graphics::Font::CharacterVertexUV) * 2);
}

void Text::RenderText(const std::string& text)
{
   mTextToRender = text;
}

}; // namespace Editor

}; // namespace CubeWorld

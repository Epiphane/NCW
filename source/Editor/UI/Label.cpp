// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Shared/Helpers/Asset.h>

#include "Label.h"

namespace CubeWorld
{

namespace Editor
{

std::unique_ptr<Engine::Graphics::Program> Label::textProgram = nullptr;
std::unique_ptr<Engine::Graphics::Program> Label::renderProgram = nullptr;

Label::Label(
   Bounded& parent,
   const Options& options
)
   : Element(parent, options)
   , mText(options.text)
   , mCallback(options.onClick)
   , mIsHovered(false)
   , mFramebuffer(GLsizei(GetWidth()), GLsizei(GetHeight()))
   , mTextVBO(Engine::Graphics::VBO::DataType::Vertices)
   , mRenderVBO(Engine::Graphics::VBO::DataType::Vertices)
{
   auto maybeFont = Engine::Graphics::FontManager::Instance()->GetFont(Asset::Font(options.font));
   assert(maybeFont);
   mFont = *maybeFont;

   if (!textProgram)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/DebugText.vert", "Shaders/DebugText.geom", "Shaders/DebugText.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading DebugText shader").GetMessage());
         return;
      }

      textProgram = std::move(*maybeProgram);

      textProgram->Attrib("aPosition");
      textProgram->Attrib("aUV");
      textProgram->Uniform("uTexture");
      textProgram->Uniform("uWindowSize");
   }

   if (!renderProgram)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/PassthroughTexture.vert", "Shaders/PassthroughTexture.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading PassthroughTexture shader").GetMessage());
         return;
      }

      renderProgram = std::move(*maybeProgram);

      renderProgram->Attrib("aPosition");
      renderProgram->Attrib("aUV");
      renderProgram->Uniform("uTexture");
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
   mRenderVBO.BufferData(GLsizei(sizeof(GLfloat) * vboData.size()), &vboData[0], GL_STATIC_DRAW);

   SetText(options.text);
}

void Label::MouseClick(int button, double x, double y)
{
   if (mCallback && ContainsPoint(x, y))
   {
      mCallback();
   }
}

void Label::MouseMove(double x, double y)
{
   if (mCallback && ContainsPoint(x, y) && !mIsHovered)
   {
      RenderText("> " + mText);
      mIsHovered = true;
   }
   else if (!ContainsPoint(x, y) && mIsHovered)
   {
      RenderText(mText);
      mIsHovered = false;
   }
}

void Label::RenderText(const std::string& text)
{
   std::vector<Engine::Graphics::Font::CharacterVertexUV> uvs = mFont->Write(0, 0, 1, text);

   mTextVBO.BufferData(sizeof(Engine::Graphics::Font::CharacterVertexUV) * uvs.size(), &uvs[0], GL_STATIC_DRAW);

   mFramebuffer.Bind();
   BIND_PROGRAM_IN_SCOPE(textProgram);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mFont->GetTexture());
   textProgram->Uniform1i("uTexture", 0);
   textProgram->Uniform2f("uWindowSize", static_cast<GLfloat>(GetWidth()), static_cast<GLfloat>(GetHeight()));

   mTextVBO.AttribPointer(textProgram->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
   mTextVBO.AttribPointer(textProgram->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

   glDrawArrays(GL_LINES, 0, uvs.size());
   mFramebuffer.Unbind();
}

void Label::Update(TIMEDELTA dt)
{
   // Draw framebuffer to the screen
   BIND_PROGRAM_IN_SCOPE(renderProgram);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mFramebuffer.GetTexture());
   renderProgram->Uniform1i("uTexture", 0);

   mRenderVBO.AttribPointer(renderProgram->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
   mRenderVBO.AttribPointer(renderProgram->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);
}

}; // namespace Editor

}; // namespace CubeWorld

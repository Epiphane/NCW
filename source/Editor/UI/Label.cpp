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
   , mText("")
   , mClickCallback(options.onClick)
   , mChangeCallback(options.onChange)
   , mIsHovered(false)
   , mIsFocused(false)
   , mKeyCallbacks{}
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

   if (mChangeCallback)
   {
      auto onAlpha = std::bind(&Label::OnAlphaKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; key++)
      {
         mKeyCallbacks.push_back(Engine::Window::Instance()->GetInput()->AddCallback(Engine::Input::Key(key), onAlpha));
         mKeyCallbacks.push_back(Engine::Window::Instance()->GetInput()->AddCallback(Engine::Input::ShiftKey(key), onAlpha));
      }
      mKeyCallbacks.push_back(Engine::Window::Instance()->GetInput()->AddCallback(Engine::Input::Key(GLFW_KEY_BACKSPACE), onAlpha));
      mKeyCallbacks.push_back(Engine::Window::Instance()->GetInput()->AddCallback(Engine::Input::Key(GLFW_KEY_ENTER), onAlpha));
   }
}

void Label::OnAlphaKey(int key, int action, int mods)
{
   if (!mIsFocused || action != GLFW_PRESS)
   {
      return;
   }

   switch (key)
   {
   case GLFW_KEY_BACKSPACE:
      if (mText.size() > 1)
      {
         // Erase the second to last character
         mText.erase(mText.end() - 2, mText.end() - 1);
         RenderText(mText);
      }
      break;
   case GLFW_KEY_ENTER:
      mIsFocused = false;
      mText.pop_back();
      mChangeCallback(mText);
      RenderText(mText);
      break;
   default:
      // It's a letter
      char ch = (key - GLFW_KEY_A) + 'a';
      if ((mods & GLFW_MOD_SHIFT) != 0)
      {
         ch += 'A' - 'a';
      }
      mText.insert(mText.end() - 1, ch);
      RenderText(mText);
   }
}

void Label::MouseClick(int button, double x, double y)
{
   if (mClickCallback && ContainsPoint(x, y))
   {
      mClickCallback();
   }
   if (mChangeCallback)
   {
      bool wasFocused = mIsFocused;
      mIsFocused = ContainsPoint(x, y);
      if (mIsFocused && !wasFocused)
      {
         mText.push_back('_');
         RenderText(mText);
      }
      else if (!mIsFocused && wasFocused)
      {
         mText.pop_back();
         mChangeCallback(mText);
         RenderText(mText);
      }
   }
}

void Label::RenderText(const std::string& text)
{
   if (text.empty())
   {
      return;
   }

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
   if (mText.empty())
   {
      return;
   }

   if (mClickCallback)
   {
      glm::tvec2<double> mouse = Engine::Window::Instance()->GetInput()->GetMousePosition();
      bool hovered = ContainsPoint((mouse.x - mParent.GetX()) / mParent.GetWidth(), (mouse.y - mParent.GetY()) / mParent.GetHeight());
      if (hovered && !mIsHovered)
      {
         RenderText("> " + mText);
         mIsHovered = true;
      }
      else if (!hovered && mIsHovered)
      {
         RenderText(mText);
         mIsHovered = false;
      }
   }

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

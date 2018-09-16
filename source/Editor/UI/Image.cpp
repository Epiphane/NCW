// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Shared/Helpers/Asset.h>
#pragma warning(push, 0)
#include <Shared/Helpers/json.hpp>
#pragma warning(pop)

#include "Image.h"

namespace CubeWorld
{

namespace Editor
{

std::unique_ptr<Engine::Graphics::Program> Image::program = nullptr;

Image::Image(
   Bounded& parent,
   const Options& options
)
   : Element(parent, options)
   , mCallback(options.onClick)
   , mIsHovered(false)
   , mIsPressed(false)
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
   
   LOG_DEBUG("Loading %1", Paths::Canonicalize(options.filename));
   Maybe<Engine::Graphics::Texture*> maybeTexture = Engine::Graphics::TextureManager::Instance()->GetTexture(options.filename);
   if (!maybeTexture)
   {
      LOG_ERROR(maybeTexture.Failure().WithContext("Failed loading %1", options.filename).GetMessage());
      return;
   }
   mTexture = *maybeTexture;

   glm::vec4 coords = mTexture->GetImage(options.image);
   glm::vec4 hover = options.hoverImage.empty() ? coords : mTexture->GetImage(options.hoverImage);
   glm::vec4 press = options.pressImage.empty() ? coords : mTexture->GetImage(options.pressImage);

   float x = 2.0f * mOptions.x - 1.0f;
   float y = 2.0f * mOptions.y - 1.0f;
   float w = 2.0f * mOptions.w;
   float h = 2.0f * mOptions.h;

   std::vector<GLfloat> vboData = {
      x,     y,     0.0f, coords.x,            coords.y + coords.w,
      x + w, y,     0.0f, coords.x + coords.z, coords.y + coords.w,
      x,     y + h, 0.0f, coords.x,            coords.y,
      x,     y + h, 0.0f, coords.x,            coords.y,
      x + w, y,     0.0f, coords.x + coords.z, coords.y + coords.w,
      x + w, y + h, 0.0f, coords.x + coords.z, coords.y,

      x,     y,     0.0f, hover.x,           hover.y + hover.w,
      x + w, y,     0.0f, hover.x + hover.z, hover.y + hover.w,
      x,     y + h, 0.0f, hover.x,           hover.y,
      x,     y + h, 0.0f, hover.x,           hover.y,
      x + w, y,     0.0f, hover.x + hover.z, hover.y + hover.w,
      x + w, y + h, 0.0f, hover.x + hover.z, hover.y,

      x,     y,     0.0f, press.x,           press.y + press.w,
      x + w, y,     0.0f, press.x + press.z, press.y + press.w,
      x,     y + h, 0.0f, press.x,           press.y,
      x,     y + h, 0.0f, press.x,           press.y,
      x + w, y,     0.0f, press.x + press.z, press.y + press.w,
      x + w, y + h, 0.0f, press.x + press.z, press.y,
   };
   mVBO.BufferData(GLsizei(sizeof(GLfloat) * vboData.size()), &vboData[0], GL_STATIC_DRAW);
}

void Image::MouseDown(int button, double x, double y)
{
   mIsPressed = ContainsPoint(x, y);
}

void Image::MouseUp(int button, double x, double y)
{
   if (mCallback && mIsPressed && ContainsPoint(x, y))
   {
      mCallback();
   }
   mIsPressed = false;
}

void Image::MouseClick(int button, double x, double y)
{
   if (mCallback && ContainsPoint(x, y))
   {
      mCallback();
   }
   mIsPressed = false;
}

void Image::MouseMove(double x, double y)
{
   mIsHovered = ContainsPoint(x, y);
}

void Image::Update(TIMEDELTA dt)
{
   // Draw framebuffer to the screen
   BIND_PROGRAM_IN_SCOPE(program);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mTexture->GetTexture());
   program->Uniform1i("uTexture", 0);

   mVBO.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
   mVBO.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));

   GLint first = 0;
   if (mIsHovered)
   {
      first = mIsPressed ? 12 : 6;
   }

   glDrawArrays(GL_TRIANGLES, first, 6);
}

}; // namespace Editor

}; // namespace CubeWorld

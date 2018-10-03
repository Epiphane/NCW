// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Paths.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>

#include "../Command/CommandStack.h"
#include "Scrubber.h"

namespace CubeWorld
{

namespace Editor
{

std::unique_ptr<Engine::Graphics::Program> BaseScrubber::program = nullptr;

BaseScrubber::BaseScrubber(
   Bounded& parent,
   const Options& options
)
   : Element(parent, options)
   , mScrubbing(nullptr)
   , mLastPosition(0)
   , mOffset(0)
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
      program->Uniform("uOffset");
   }
   
   LOG_DEBUG("Loading %1", Paths::Canonicalize(options.filename));
   Maybe<Engine::Graphics::Texture*> maybeTexture = Engine::Graphics::TextureManager::Instance()->GetTexture(options.filename);
   if (!maybeTexture)
   {
      LOG_ERROR(maybeTexture.Failure().WithContext("Failed loading %1", options.filename).GetMessage());
      return;
   }
   mTexture = *maybeTexture;

   glm::vec4 coords(0, 0, 1, 1);
   if (!options.image.empty())
   {
      coords = mTexture->GetImage(options.image);
   }

   float h = 2.0f * mOptions.h;
   float w = 2.0f * mOptions.w;
   float x = 2.0f * mOptions.x - 1.0f;
   float y = 2.0f * mOptions.y - 1.0f;

   std::vector<GLfloat> vboData = {
      x,     y,     mOptions.z, coords.x,            coords.y + coords.w,
      x + w, y,     mOptions.z, coords.x + coords.z, coords.y + coords.w,
      x,     y + h, mOptions.z, coords.x,            coords.y,
      x,     y + h, mOptions.z, coords.x,            coords.y,
      x + w, y,     mOptions.z, coords.x + coords.z, coords.y + coords.w,
      x + w, y + h, mOptions.z, coords.x + coords.z, coords.y,
   };
   mVBO.BufferData(GLsizei(sizeof(GLfloat) * vboData.size()), &vboData[0], GL_STATIC_DRAW);
}

void BaseScrubber::MouseDown(int button, double x, double y)
{
   if (button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return;
   }

   if (ContainsPoint(x, y))
   {
      mLastPosition = glm::tvec2<double>((x - mOptions.x) / mOptions.w, (y - mOptions.y) / mOptions.h);
      StartScrubbing();
   }
}

void BaseScrubber::MouseUp(int button, double, double)
{
   if (button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return;
   }

   if (mScrubbing)
   {
      // Funky time: at this point, the current value represents the NEW state,
      // and mScrubbing represents a command to set it to the OLD state. So we
      // perform the command twice, once immediately to revert to the old state,
      // and then again when it gets placed on the stack to go back to the new
      // state.
      mScrubbing->Do();
      CommandStack::Instance()->Do(std::move(mScrubbing));
   }
}

void BaseScrubber::Update(TIMEDELTA)
{
   if (mScrubbing)
   {
      glm::tvec2<double> mouse = AbsoluteToRelative(Engine::Window::Instance()->GetInput()->GetRawMousePosition());
      Scrub(mouse.x - mLastPosition.x);
      mLastPosition = mouse;
   }

   // Draw framebuffer to the screen
   BIND_PROGRAM_IN_SCOPE(program);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mTexture->GetTexture());
   program->Uniform1i("uTexture", 0);
   program->UniformVector3f("uOffset", mOffset);

   mVBO.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
   mVBO.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));

   GLint first = 0;
   glDrawArrays(GL_TRIANGLES, first, 6);
}

}; // namespace Editor

}; // namespace CubeWorld

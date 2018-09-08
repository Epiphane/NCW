// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/Framebuffer.h>
#include <Engine/Graphics/VBO.h>

namespace CubeWorld
{

namespace Editor
{

//
// Manages a subsection of the Editor. Notably, allows for hooking up different framebuffers,
// binding and unbinding them in an understandable way, and re-rendering those pieces into
// the space they belong.
//
class SubWindow : public Bounded
{
public:
   struct Options {
      float x = 0;
      float y = 0;
      float w = 1;
      float h = 1;
   };

public:
   SubWindow(
      Bounded& parent,
      const Options& options
   );
   ~SubWindow();

   //
   // Bind for rendering to a specific framebuffer.
   //
   void Bind();

   //
   // Analog to unbind
   //
   void Unbind();

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Render();

   uint32_t Width() const override { return mParent.Width() * mOptions.w; }
   uint32_t Height() const override { return mParent.Height() * mOptions.h; }

private:
   Bounded& mParent;
   Options mOptions;

private:
   Engine::Graphics::Framebuffer mFramebuffer;
   Engine::Graphics::VBO mVBO;

private:
   static GLuint program;
   static GLuint aPosition, aUV;
   static GLuint uTexture;
};

}; // namespace Editor

}; // namespace CubeWorld

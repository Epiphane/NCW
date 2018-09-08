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
      uint32_t x = 0;
      uint32_t y = 0;
      uint32_t w = 1280;
      uint32_t h = 760;
   };

public:
   SubWindow(
      Bounded* bounds,
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

   uint32_t Width() const override { return mOptions.w; }
   uint32_t Height() const override { return mOptions.h; }

private:
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

// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

#include "Element.h"

namespace CubeWorld
{

namespace Editor
{

//
// Manages a subsection of the Editor. Notably, allows for hooking up different framebuffers,
// binding and unbinding them in an understandable way, and re-rendering those pieces into
// the space they belong.
//
class Image : public Element
{
public:
   struct Options : public Element::Options {
      std::string filename;
      std::string image;
      std::string hoverImage;
      std::string pressImage;
      std::function<void(void)> onClick;
   };

public:
   Image(
      Bounded& parent,
      const Options& options
   );

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override;

   void MouseMove(double x, double y) override;
   void MouseDown(int button, double x, double y) override;
   void MouseUp(int button, double x, double y) override;
   void MouseClick(int button, double x, double y) override;

private:
   std::function<void(void)> mCallback;
   bool mIsHovered;
   bool mIsPressed;

private:
   Engine::Graphics::Texture* mTexture;
   Engine::Graphics::VBO mVBO;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Editor

}; // namespace CubeWorld

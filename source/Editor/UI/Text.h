// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/Framebuffer.h>
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
class Text : public Element
{
public:
   struct Options : public Element::Options {
      std::string text;
      std::string font = "debug";
   };

public:
   Text(
      Bounded& parent,
      const Options& options
   );

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override;

   //
   // Render the text on this label
   //
   void SetText(const std::string& text)
   {
      if (text == mText)
      {
         return;
      }

      mText = text;
      RenderText(text);
   }

protected:
   void RenderText(const std::string& text);

   std::string mText;

private:
   Engine::Graphics::Framebuffer mFramebuffer;
   Engine::Graphics::Font* mFont;
   Engine::Graphics::VBO mTextVBO;
   Engine::Graphics::VBO mRenderVBO;

private:
   static std::unique_ptr<Engine::Graphics::Program> textProgram;
   static std::unique_ptr<Engine::Graphics::Program> renderProgram;
};

}; // namespace Editor

}; // namespace CubeWorld

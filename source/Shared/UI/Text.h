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
#include <Engine/UI/UIElement.h>
#include "../Aggregator/Text.h"

namespace CubeWorld
{

namespace UI
{

//
// Generic element that contains text. It can be mutable (see TextField), or immutable.
//
class Text : public Engine::UIElement
{
public:
   struct Options {
      Options() = default;
      Options(const std::string& text) : text(text) {};
      Options(const Options& other) : text(other.text), font(other.font), size(other.size), alignment(other.alignment) {};

      std::string text = "";
      std::string font = "debug";
      uint32_t size = 0;
      Engine::Graphics::Font::Alignment alignment = Engine::Graphics::Font::Left;

      virtual uint32_t DefaultSize() const { return uint32_t(text.size()); }
   };

   Text(Engine::UIRoot* root, UIElement* parent, const Options& options, const std::string& name = "");

   //
   // Render the text on this label
   //
   void SetText(const std::string& text);

   void SetAlignment(Engine::Graphics::Font::Alignment newAlignment);
   Engine::Graphics::Font::Alignment GetAlignment();
   
   // Adds a constraint to this element such that it will take up as much space as its rendered text
   void ConstrainLayoutWidthToContentWidth();
   void ConstrainLayoutHeightToContentHeight();

   void Redraw() override;

protected:
   void RenderText(const std::string& text);
   void RecalculateSize();

   Engine::Graphics::Font* mFont;

   std::string mText;
   // Could be different from mText, see TextButton
   std::string mRendered;

   Aggregator::Text::Region mRegion;

   Engine::Graphics::Font::Alignment mAlignment;

   // Edit variable that lets you make constraints to the rendered text size.
   //    Will change their values whenever new text or new fonts arrive.
   rhea::variable mContentSizeX;
   rhea::variable mContentSizeY;
};

}; // namespace UI

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Aggregator/Text.h>
#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/Framebuffer.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/UI/UIElement.h>

namespace CubeWorld
{

namespace Editor
{

//
// Manages a subsection of the Editor. Notably, allows for hooking up different framebuffers,
// binding and unbinding them in an understandable way, and re-rendering those pieces into
// the space they belong.
//
class Text : public Engine::UIElement
{
public:
   struct Options {
      std::string text = "";
      std::string font = "debug";
      uint32_t size = 0;
   };

public:
   Text(Engine::UIRoot* root, UIElement* parent, const Options& options);

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

   void Receive(const Engine::UIRebalancedEvent& evt) override;

protected:
   void RenderText(const std::string& text);

   std::string mText;

   // Could be different from mText, see TextButton
   std::string mRendered;

private:
   Engine::Graphics::Font* mFont;

private:
   Engine::Aggregator::Text::Region mRegion;
};

}; // namespace Editor

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/Framebuffer.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

#include "Text.h"

namespace CubeWorld
{

namespace Editor
{

//
// A button that's text, instead of an image
//
class TextButton : public Text
{
public:
   struct Options : public Text::Options {
      Options() = default;
      Options(
         const Text::Options& base,
         std::function<void(void)> onClick
      )
         : Text::Options(base)
         , onClick(onClick)
      {};

      std::function<void(void)> onClick;
   };

public:
   TextButton(Engine::UIRoot* root, UIElement* parent, const Options& options);

   void Update(TIMEDELTA dt) override;

   void Receive(const MouseClickEvent& evt);

private:
   std::function<void(void)> mClickCallback;
   bool mIsHovered;
};

}; // namespace Editor

}; // namespace CubeWorld
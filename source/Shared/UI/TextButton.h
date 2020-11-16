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

namespace UI
{

//
// A button that's text, instead of an image
//
class TextButton : public Text
{
public:
   struct Options : public Text::Options {
      Options() = default;
      Options(const Text::Options& base, std::function<void(void)> onClick)
         : Text::Options(base)
         , onClick(onClick)
      {};

      std::function<void(void)> onClick;

   public:
      virtual uint32_t DefaultSize() const override { return uint32_t(text.size()) + 2; }
   };

public:
   TextButton(Engine::UIRootDep* root, UIElementDep* parent, const Options& options, const std::string& name = "");

   void Focus();
   void Unfocus();

   Action MouseClick(const MouseClickEvent& evt) override;

   void Update(TIMEDELTA dt) override;

private:
   std::function<void(void)> mClickCallback;

   bool mIsHovered;
   bool mIsFocused;
};

}; // namespace UI

}; // namespace CubeWorld

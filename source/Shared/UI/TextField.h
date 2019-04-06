// By Thomas Steinke

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/Input.h>
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
// A label that can be edited by selecting and typing.
//
class TextField : public Text
{
public:
   struct Options : public Text::Options {
      Options() = default;
      Options(std::function<void(std::string)> onChange) : Text::Options(), onChange(onChange) {};
      Options(const Text::Options& base, std::function<void(std::string)> onChange)
         : Text::Options(base)
         , onChange(onChange)
      {};

      std::function<void(std::string)> onChange;

   public:
      virtual uint32_t DefaultSize() const override { return std::max(16U, uint32_t(text.size())); }
   };

public:
   TextField(Engine::UIRoot* root, Engine::UIElement* element, const Options& options);

   Action MouseClick(const MouseClickEvent& evt) override;

private:
   std::function<void(std::string)> mChangeCallback;
   bool mIsFocused;

   std::vector<std::unique_ptr<Engine::Input::KeyCallbackLink>> mKeyCallbacks;

   void AddCharacter(char ch);
   void OnAlphaKey(int key, int action, int mods);
};

}; // namespace UI

}; // namespace CubeWorld

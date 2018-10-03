// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/Input.h>
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
// A label that can be edited by selecting and typing.
//
class TextField : public Text
{
public:
   struct Options : public Text::Options {
      Options() = default;
      Options(
         const Text::Options& base,
         std::function<void(std::string)> onChange
      )
         : Text::Options(base)
         , onChange(onChange)
      {};

      std::function<void(std::string)> onChange;
   };

public:
   TextField(
      Bounded& parent,
      const Options& options
   );

   void MouseClick(int button, double x, double y) override;

private:
   std::function<void(std::string)> mChangeCallback;
   bool mIsFocused;

   std::vector<std::unique_ptr<Engine::Input::KeyCallbackLink>> mKeyCallbacks;
   void OnAlphaKey(int key, int action, int mods);
};

}; // namespace Editor

}; // namespace CubeWorld

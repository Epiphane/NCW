// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/UI/Binding.h>

#include "Text.h"

namespace CubeWorld
{

namespace Editor
{

//
//
//
template <typename N>
class NumDisplay : public Text, public Engine::Binding<N>
{
public:
   struct Options : public Text::Options {
      uint8_t precision = 2;

   public:
      // Allow for, by default, ####.{precision}
      virtual uint32_t DefaultSize() const override { return precision + 5; }
   };

public:
   NumDisplay(Bounded& parent, const Options& options)
      : Text(parent, options)
      , mFormat("%." + Format::FormatString("%1", options.precision) + "f")
   {};

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override
   {
      if (Binding<N>::Update())
      {
         SetText(Format::FormatString(mFormat, this->GetValue()));
      }

      Text::Update(dt);
   }

private:
   std::string mFormat;
};

}; // namespace Editor

}; // namespace CubeWorld

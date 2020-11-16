// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <RGBDesignPatterns/Command.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/UI/Binding.h>
#include <Engine/UI/UIElementDep.h>
#include <Engine/UI/UIRootDep.h>

#include "Image.h"

namespace CubeWorld
{

namespace UI
{

//
// This is just convenience, so I can hide some implementation in Scrubber.cpp. Always use the derived Scrubber<N> class
//
class ScrollBar : public Image, public Engine::Binding<double>
{
public:
   struct Options : public Image::Options {
      double min = 0;
      double max = 1;
      double* binding = nullptr;
      std::function<void(double)> onChange = nullptr;
   };

public:
   ScrollBar(Engine::UIRootDep* root, Engine::UIElementDep* parent, const Options& options);

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override;

   Action MouseDown(const MouseDownEvent& evt) override;
   Action MouseUp(const MouseUpEvent& evt) override;

   bool IsScrubbing() { return mScrubbing; }

   void SetBounds(double min, double max)
   {
      mMin = min;
      mRange = max - min;
   }

private:
   void Redraw() override;

protected:
   // State
   bool mScrubbing;

private:
   // Configuration
   double mMin, mRange;
   std::function<void(double)> mCallback;
};

}; // namespace UI

}; // namespace CubeWorld

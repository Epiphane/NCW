// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/Command.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/UI/Binding.h>
#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>

#include "../Aggregator/Image.h"

namespace CubeWorld
{

namespace UI
{

//
// This is just convenience, so I can hide some implementation in Scrubber.cpp. Always use the derived Scrubber<N> class
//
class ScrollBar : public Engine::UIElement, public Engine::Binding<double>
{
public:
   struct Options {
      std::string filename;
      std::string image = "";
      double min = 0;
      double max = 1;
      double* binding = nullptr;
      std::function<void(double)> onChange = nullptr;
   };

public:
   ScrollBar(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options);

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override;

   void Receive(const MouseDownEvent& evt);
   void Receive(const MouseUpEvent& evt);

   bool IsScrubbing() { return mScrubbing; }

   void SetBounds(double min, double max)
   {
      mMin = min;
      mRange = max - min;
   }

private:
   void UpdateRegion();

public:
   void Receive(const Engine::UIRebalancedEvent& evt) override;

protected:
   // State
   bool mScrubbing;

private:
   // Configuration
   double mMin, mRange;
   std::function<void(double)> mCallback;

private:
   Engine::Graphics::Texture* mTexture;
   glm::vec4 mCoords;

private:
   Aggregator::Image::Region mRegion;
};

}; // namespace UI

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/Command.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

#include "Binding.h"
#include "Element.h"

namespace CubeWorld
{

namespace Editor
{

//
// This is just convenience, so I can hide some implementation in Scrubber.cpp. Always use the derived Scrubber<N> class
//
class ScrollBar : public Element, public Binding<double>
{
public:
   struct Options : public Element::Options {
      std::string filename;
      std::string image = "";
      double min = 0;
      double max = 1;
      double* binding = nullptr;
      std::function<void(double)> onChange = nullptr;
   };

public:
   ScrollBar(
      Bounded& parent,
      const Options& options
   );

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override;

   void MouseDown(int button, double x, double y) override;
   void MouseUp(int button, double x, double y) override;

   bool IsScrubbing() { return mScrubbing; }

   void SetBounds(double min, double max)
   {
      mMin = min;
      mRange = max - min;
   }

protected:
   // State
   bool mScrubbing;
   glm::vec3 mOffset;

private:
   // Configuration
   double mMin, mRange;
   std::function<void(double)> mCallback;

private:
   Engine::Graphics::Texture* mTexture;
   Engine::Graphics::VBO mVBO;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Editor

}; // namespace CubeWorld

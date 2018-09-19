// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

#include "Element.h"

namespace CubeWorld
{

namespace Editor
{

//
// Manages a subsection of the Editor. Notably, allows for hooking up different framebuffers,
// binding and unbinding them in an understandable way, and re-rendering those pieces into
// the space they belong.
//
class Scrubber : public Element
{
public:
   struct Options : public Element::Options {
      std::string filename;
      std::string image = "";
      std::function<void(double)> onPress = nullptr;
      std::function<void(double)> onMove = nullptr;
      std::function<void(double)> onRelease = nullptr;
   };

public:
   Scrubber(
      Bounded& parent,
      const Options& options
   );

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override;

   void MouseDown(int button, double x, double y) override;
   void MouseUp(int button, double x, double y) override;
   void MouseDrag(int button, double x, double y) override;

   void SetValue(double value);

private:
   std::function<void(double)> mPressCallback, mMoveCallback, mReleaseCallback;
   bool mIsPressed;

   double mMin, mValue, mMax;

private:
   Engine::Graphics::Texture* mTexture;
   Engine::Graphics::VBO mVBO;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Editor

}; // namespace CubeWorld

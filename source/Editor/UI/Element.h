// By Thomas Steinke

#pragma once

#include <Engine/Core/Config.h>
#include <Engine/Core/Bounded.h>

namespace CubeWorld
{

namespace Editor
{

//
// Manages a subsection of the Editor. Notably, allows for hooking up different framebuffers,
// binding and unbinding them in an understandable way, and re-rendering those pieces into
// the space they belong.
//
class Element : public Bounded
{
public:
   struct Options {
      float x = 0;
      float y = 0;
      float z = 0;
      float w = 1;
      float h = 1;
   };

public:
   Element(
      Bounded& parent,
      const Options& options
   );
   ~Element();

   virtual void Update(TIMEDELTA dt) = 0;

   //
   // Mouse events. Each line has its own comment to allow for Visual Studio to hint about
   // the constraints on x and y.
   //

   // React to the mouse being pressed. x and y are relative to the parent's bounds.
   virtual void MouseDown(int button, double x, double y);
   // React to the mouse being released. x and y are relative to the parent's bounds.
   virtual void MouseUp(int button, double x, double y);
   // React to the mouse being clicked. x and y are relative to the parent's bounds.
   virtual void MouseClick(int button, double x, double y);
   // React to the mouse dragging. x and y are relative to the parent's bounds.
   virtual void MouseDrag(int button, double x, double y);

   // Get the absolute x value of this element, in pixels.
   uint32_t GetX() const override { return uint32_t(mParent.GetX() + mParent.GetWidth() * mOptions.x); }
   // Get the absolute y value of this element, in pixels.
   uint32_t GetY() const override { return uint32_t(mParent.GetY() + mParent.GetHeight() * mOptions.y); }
   // Get the width of this element, in pixels.
   uint32_t GetWidth() const override { return uint32_t(mParent.GetWidth() * mOptions.w); }
   // Get the height of this element, in pixels.
   uint32_t GetHeight() const override { return uint32_t(mParent.GetHeight() * mOptions.h); }

   bool IsActive() { return mActive; }
   void SetActive(bool active) { mActive = active; }

protected:
   bool mActive;

protected:
   // Returns whether the point [x, y] in parent space
   // is contained within this element.
   bool ContainsPoint(double x, double y);

protected:
   Bounded& mParent;
   Options mOptions;
};

}; // namespace Editor

}; // namespace CubeWorld
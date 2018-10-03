// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/Framebuffer.h>
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
class StateWindow : public Element
{
public:
   StateWindow(
      Bounded& parent,
      const Options& options
   );
   ~StateWindow();

   //
   // Render the State.
   //
   void Update(TIMEDELTA dt) override;

   //
   // React to mouse events. A SubWindow passes the event down to all its children.
   //
   void MouseDown(int button, double x, double y) override;
   void MouseUp(int button, double x, double y) override;
   void MouseClick(int button, double x, double y) override;

   //
   // Construct a child and add it to the window
   //
   template <typename E, typename ...Args>
   void Add(Args&& ...args)
   {
      AddChild(std::make_unique<E>(std::forward<Args>(args) ...));
   }

private:
   Engine::Graphics::Framebuffer mFramebuffer;
   Engine::Graphics::VBO mVBO;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Editor

}; // namespace CubeWorld

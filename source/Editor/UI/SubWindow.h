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
class SubWindow : public Element
{
public:
   SubWindow(
      Bounded& parent,
      const Options& options
   );
   ~SubWindow();

   //
   // Render the SubWindow.
   //
   void Update(TIMEDELTA dt) override;

   //
   // React to mouse events. A SubWindow passes the event down to all its children.
   //
   void MouseDown(int button, double x, double y) override;
   void MouseUp(int button, double x, double y) override;
   void MouseClick(int button, double x, double y) override;
   void MouseDrag(int button, double x, double y) override;
   void MouseMove(double x, double y) override;

   //
   // Construct a child and add it to the window
   //
   template <typename E, typename ...Args>
   E* Add(Args&& ...args)
   {
      return static_cast<E*>(AddChild(std::make_unique<E>(*this, std::forward<Args>(args) ...)));
   }

protected:
   //
   // Add an element to this window
   //
   virtual Element* AddChild(std::unique_ptr<Element>&& element);

protected:
   std::vector<std::unique_ptr<Element>> mChildren;

   Engine::Graphics::Framebuffer mFramebuffer;
   Engine::Graphics::VBO mVBO;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Editor

}; // namespace CubeWorld

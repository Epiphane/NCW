// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/Framebuffer.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

#include "SubWindow.h"

namespace CubeWorld
{

namespace Editor
{

//
// Manages a subsection of the Editor. Notably, allows for hooking up different framebuffers,
// binding and unbinding them in an understandable way, and re-rendering those pieces into
// the space they belong.
//
class SubWindowSwapper : public SubWindow
{
public:
   SubWindowSwapper(
      Bounded& parent,
      const Options& options
   ) 
      : SubWindow(parent, options)
      , current(nullptr)
   {}

   //
   // Construct a child and add it to the window
   //
   template <typename E, typename ...Args>
   E* Add(Args&& ...args)
   {
      static_assert(std::is_base_of<SubWindow, E>::value, "Only subclasses of SubWindow may be added to a SubWindowSwapper");
      E* result = static_cast<E*>(AddChild(std::make_unique<E>(*this, std::forward<Args>(args) ...)));
      result->SetActive(false);
      return result;
   }

   SubWindow* GetCurrent() { return current; }

   //
   // Swap to another SubWindow
   //
   void Swap(SubWindow* window);

private:
   SubWindow* current;
};

}; // namespace Editor

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

#include "Scrubber.h"

namespace CubeWorld
{

namespace Editor
{

//
// Manages a subsection of the Editor. Notably, allows for hooking up different framebuffers,
// binding and unbinding them in an understandable way, and re-rendering those pieces into
// the space they belong.
//
class StationaryScrubber : public Scrubber
{
public:
   StationaryScrubber(
      Bounded& parent,
      const Options& options
   );

   void MouseDrag(int button, double x, double y) override;

   void SetValue(double value) override;
};

}; // namespace Editor

}; // namespace CubeWorld

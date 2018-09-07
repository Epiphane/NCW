// By Thomas Steinke

#pragma once

#include <cstdint>

namespace CubeWorld
{

// Description for something with a width and a height.
class Bounded
{
public:
   virtual uint32_t Width() const = 0;
   virtual uint32_t Height() const = 0;
};

}; // namespace CubeWorld

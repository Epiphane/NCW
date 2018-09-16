// By Thomas Steinke

#pragma once

#include <cstdint>

namespace CubeWorld
{

// Description for something with a width and a height.
class Bounded
{
public:
   virtual uint32_t GetX() const = 0;
   virtual uint32_t GetY() const = 0;
   virtual uint32_t GetWidth() const = 0;
   virtual uint32_t GetHeight() const = 0;
};

}; // namespace CubeWorld

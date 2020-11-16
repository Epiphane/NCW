// By Thomas Steinke

#pragma once

#include <cstdint>

namespace CubeWorld
{

// Description for something with a width and a height.
class Bounded
{
public:
   virtual ~Bounded() = default;

   virtual uint32_t GetX() const = 0;
   virtual uint32_t GetY() const = 0;
   virtual uint32_t GetWidth() const = 0;
   virtual uint32_t GetHeight() const = 0;
};

struct Bounds : public Bounded
{
public:
   Bounds(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h)
      : x(x)
      , y(y)
      , w(w)
      , h(h)
   {};
   Bounds(const Bounded& bounds)
       : x(bounds.GetX())
       , y(bounds.GetY())
       , w(bounds.GetWidth())
       , h(bounds.GetHeight())
   {}

public:
   uint32_t GetX() const override { return x; }
   uint32_t GetY() const override { return y; }
   uint32_t GetWidth() const override { return w; }
   uint32_t GetHeight() const override { return h; }

protected:
   uint32_t x, y, w, h;
};

}; // namespace CubeWorld

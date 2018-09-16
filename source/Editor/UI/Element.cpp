// By Thomas Steinke

#include <cassert>

#include "Element.h"

namespace CubeWorld
{

namespace Editor
{

Element::Element(
   Bounded& parent,
   const Options& options)
   : mOptions(options)
   , mParent(parent)
{
}

Element::~Element()
{
}

bool Element::ContainsPoint(double x, double y)
{
   return x >= mOptions.x
      && x <= mOptions.x + mOptions.w
      && y >= mOptions.y
      && y <= mOptions.y + mOptions.h;
}

}; // namespace Editor

}; // namespace CubeWorld

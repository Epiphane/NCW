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
   : mActive(true)
   , mParent(parent)
   , mOptions(options)
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

void Element::MouseDown(int, double, double)
{}

void Element::MouseUp(int, double, double)
{}

void Element::MouseClick(int, double, double)
{}

void Element::MouseDrag(int, double, double)
{}

}; // namespace Editor

}; // namespace CubeWorld

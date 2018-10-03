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

glm::tvec2<double> Element::AbsoluteToRelative(glm::tvec2<double> mouse)
{
   return glm::tvec2<double>(
      (mouse.x - GetX()) / GetWidth(),
      (mouse.y - GetY()) / GetHeight()
   );
}

void Element::MouseDown(int, double, double)
{}

void Element::MouseUp(int, double, double)
{}

void Element::MouseClick(int, double, double)
{}

}; // namespace Editor

}; // namespace CubeWorld

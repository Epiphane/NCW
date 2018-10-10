//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke

#include <Engine/UI/UIRoot.h>

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

UIElement::UIElement(UIRoot* root, UIElement* parent)
   : mpRoot(root)
   , mpParent(parent)
{
   if (parent != nullptr)
   mpRoot->Subscribe<UIRebalancedEvent>(*this);
}

void UIElement::Update(TIMEDELTA dt)
{
   for (auto& child : mChildren) {
      child->Update(dt);
   }
}

void UIElement::AddConstraint(std::string nameKey, const rhea::constraint& constraint)
{
   mConstraints[nameKey] = constraint;
   
   // TODO send this to mpRoot. Using an event maybe?
}

bool UIElement::ContainsPoint(double x, double y)
{
   return
      x >= mFrame.left.int_value() &&
      x <= mFrame.right.int_value() &&
      y <= mFrame.top.int_value() &&
      y >= mFrame.bottom.int_value();
}

   
}; // namespace Engine

}; // namespace CubeWorld

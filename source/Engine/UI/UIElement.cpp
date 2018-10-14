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

UIElement* UIElement::AddChild(std::unique_ptr<UIElement>&& ptr)
{
   UIElement* element = ptr.get();

   mChildren.push_back(std::move(ptr));
   mpRoot->Emit<ElementAddedEvent>(element);
   element->ConstrainAbove(this, rhea::strength::weak());

   return element;
}

void UIElement::Receive(const UIRebalancedEvent&)
{
   Redraw();
}

void UIElement::SetActive(bool active)
{
   mActive = active;
   for (auto& child : mChildren)
   {
      child->SetActive(active);
   }
   Redraw();
}

void UIElement::Update(TIMEDELTA dt)
{
   for (auto& child : mChildren) {
      child->Update(dt);
   }
}

void UIElement::ConstrainAbove(UIElement* other, rhea::strength strength)
{
   mpRoot->AddConstraints({
      rhea::constraint(mFrame > *other, strength)
   });
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

rhea::linear_inequality operator>(UIElement& lhs, UIElement& rhs)
{
   return lhs.GetFrame() > rhs.GetFrame();
}

rhea::linear_inequality operator>(UIElement& lhs, UIFrame& rhs)
{
   return lhs.GetFrame() > rhs;
}

rhea::linear_inequality operator>(UIFrame& lhs, UIElement& rhs)
{
   return lhs > rhs.GetFrame();
}

rhea::linear_inequality operator>(UIFrame& lhs, UIFrame& rhs)
{
   return rhs.z >= lhs.z + 0.01;
}
   
}; // namespace Engine

}; // namespace CubeWorld

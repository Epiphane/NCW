//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke

#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

UIElement::UIElement(UIRoot* root, UIElement* parent)
   : UIConstrainable(root)
   , mActive(true)
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

   UIFrame& fChild = element->GetFrame();

   mpRoot->AddConstraints({
      rhea::constraint(fChild > mFrame, rhea::strength::weak()),
   });

   return element;
}

void UIElement::Receive(const UIRebalancedEvent&)
{
   Redraw();
}

void UIElement::SetActive(bool active)
{
   if (active == mActive)
   {
      return;
   }

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

void UIElement::ConstrainInFrontOf(UIElement* other, rhea::strength strength)
{
   mpRoot->AddConstraints({
      rhea::constraint(mFrame > *other, strength)
   });
}

void UIElement::Contains(UIElement* other, rhea::strength strength)
{
   UIFrame& fOther = other->GetFrame();
   mpRoot->AddConstraints({
      rhea::constraint(mFrame.left <= fOther.left, strength),
      rhea::constraint(mFrame.right >= fOther.right, strength),
      rhea::constraint(mFrame.top >= fOther.top, strength),
      rhea::constraint(mFrame.bottom <= fOther.bottom, strength),
   });
}

void UIElement::AddConstraint(std::string nameKey, const rhea::constraint& constraint)
{
   mConstraints[nameKey] = constraint;
   
   mpRoot->AddConstraints({mConstraints[nameKey]});
   
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
   return rhs.z >= lhs.z + 1.0;
}
   
}; // namespace Engine

}; // namespace CubeWorld

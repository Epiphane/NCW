//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke

#include <sstream>

#include <RGBText/Format.h>
#include <RGBText/StringHelper.h>
#include <RGBLogger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

UIElement::UIElement(UIRoot *root, UIElement* parent, const std::string& name)
   : UIConstrainable(root, name)
   , mActive(true)
   , mpParent(parent)
{
   if (parent != nullptr)
      mpRoot->Subscribe<UIRebalancedEvent>(*this);

   mBag = std::make_shared<Observables::DisposeBag>();
}

UIElement::~UIElement()
{
   if (mpRoot && mpRoot != this)
   {
      mpRoot->ElementDestructing(this);
   }
}

UIElement* UIElement::AddChild(std::unique_ptr<UIElement>&& ptr)
{
   UIElement* element = ptr.get();

   mChildren.push_back(std::move(ptr));
   mpRoot->Emit<ElementAddedEvent>(element);

   UIConstraint::Options constraintOptions;
   constraintOptions.priority = UIConstraint::LOW_PRIORITY;
   element->ConstrainInFrontOf(this, constraintOptions);

   mpRoot->AddConstraints({mFrame.biggestDescendantZ >= element->GetFrame().biggestDescendantZ});

   return element;
}

//
// Destroy a child of this element and release its memory.
//    Also recursively destroys children of this child.
//
void UIElement::DestroyChild(UIElement* childToDestroy)
{
   auto filteringLambda = [childToDestroy](const auto& child) { return child.get() == childToDestroy; };

   mChildren.erase(std::remove_if(mChildren.begin(), mChildren.end(), filteringLambda), mChildren.end());

   // Destructor of childToDestroy AND ALL ITS CHILDREN will now be called.
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

   mActiveObservable.SendMessage(active);

   Redraw();
}

void UIElement::Update(TIMEDELTA dt)
{
   for (auto& child : mChildren) {
      child->Update(dt);
   }
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

bool UIElement::ContainsPoint(double x, double y) const
{
   return
      x >= mFrame.left.int_value() &&
      x <= mFrame.right.int_value() &&
      y <= mFrame.top.int_value() &&
      y >= mFrame.bottom.int_value();
}

UIElement::DebugInfo UIElement::GetDebugInfo(bool bRecursive)
{
   DebugInfo result;

   result.name = GetName();
   result.origin.x = GetX();
   result.origin.y = GetY();
   result.size.x = GetWidth();
   result.size.y = GetHeight();
   result.z = GetFrame().z.value();
   result.maxZ = GetFrame().biggestDescendantZ.value();

   if (bRecursive) {
      for (const auto& child : mChildren)
      {
         result.children.push_back(child->GetDebugInfo(true));
      }
   }

   return result;
}

void UIElement::LogDebugInfo(bool bRecursive, uint32_t indentLevel)
{
   Logger::LogManager& logger = Logger::LogManager::Instance();

   std::string indentation;
   indentation.insert(0, indentLevel * 2, ' ');

   DebugInfo my = GetDebugInfo(false);

   // First line has multiple colors
   logger.Log("DEBUG | ");
   logger.Log(indentation.c_str());
   logger.Log(my.name.c_str(), Logger::Logger::Red);
   logger.Log(FormatString(" [{type}]\n", my.type).c_str());

   // The rest is pretty simple.
   logger.Log(Logger::LogLevel::kDebug, "{}Origin: ({x}, {y}) Size: ({w}, {h})", indentation, my.origin.x, my.origin.y, my.size.x, my.size.y);
   logger.Log(Logger::LogLevel::kDebug, "{}Z: {z} Biggest Child Z: {maxZ}", indentation, my.z, my.maxZ);

   if (bRecursive) {
      if (mChildren.size() == 0)
      {
         logger.Log(Logger::LogLevel::kDebug, "{}Children: None", indentation);
      }
      else
      {
         logger.Log(Logger::LogLevel::kDebug, "{}Children: [", indentation);
         for (const auto& child : mChildren)
         {
            child->LogDebugInfo(true, indentLevel + 1);
         }
         logger.Log(Logger::LogLevel::kDebug, "{}]", indentation);
      }
   }
}

void UIElement::InitFromJSON(const BindingProperty& /*data*/)
{
}

BindingProperty UIElement::ConvertToJSON()
{
   BindingProperty result;
   result["class"] = GetDebugInfo().type;
   result["name"]  = GetName();

   for (const auto& child : mChildren) {
      result["children"].push_back(child->ConvertToJSON());
   }

   return result;
}

bool UIElement::IsMarkedForDeletion() const
{
   return mbDeleteAfterThisFrame;
}

UIElement* UIElement::GetParent() const
{
   return mpParent;
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
   return lhs.z >= rhs.z + 1.0;
}

UIElement::Action UIElement::MouseDown(const MouseDownEvent& evt)
{
   bool wantsToCapture = false;
   for (auto& g : mGestureRecognizers) {
      wantsToCapture = g->MouseDown(evt) || wantsToCapture;
   }

   if (wantsToCapture) {
      return Capture;
   }

   return mbAbsorbsMouseEvents ? Handled : Unhandled;
}

UIElement::Action UIElement::MouseMove(const MouseMoveEvent& evt)
{
   bool wantsToCapture = false;
   for (auto& g : mGestureRecognizers) {
      wantsToCapture = g->MouseMove(evt) || wantsToCapture;
   }

   if (wantsToCapture) {
      return Capture;
   }

   return mbAbsorbsMouseEvents ? Handled : Unhandled;
}

UIElement::Action UIElement::MouseUp(const MouseUpEvent& evt)
{
   for (auto& g : mGestureRecognizers) {
      g->MouseUp(evt);
   }

   return mbAbsorbsMouseEvents ? Handled : Unhandled;
}

}; // namespace Engine

}; // namespace CubeWorld

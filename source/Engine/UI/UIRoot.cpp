// By Thomas Steinke

#include <utility>

#include <Engine/Logger/Logger.h>
#include <Engine/Graphics/Program.h>
#include <Shared/Helpers/TimSort.h>

//#include <rhea/rhea/iostream.hpp> // Uncomment if you want to do something like `cout << rhea::variable`

#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{

UIRoot::UIRoot()
   : UIElement(this, nullptr, "Root")
   , mBoundConstraints{}
   , mDirty(false)
{
   // Disable autosolve, otherwise we try to solve whenever we add a new constraint
   mSolver.set_autosolve(false);
   mSolver.on_resolve = [&](rhea::simplex_solver&) {
      mDirty = true;
   };
   mSolver.on_variable_change = [&](const rhea::variable&, rhea::solver&) {
      // TODO maybe drill down and find the element that cares one day.
      mDirty = true;
   };

   AddConstraintsForElement(mFrame);
   Subscribe<ElementAddedEvent>(*this);
   Subscribe<ElementRemovedEvent>(*this);
   Subscribe<MouseMoveEvent>(*this);
   Subscribe<MouseDownEvent>(*this);
   Subscribe<MouseUpEvent>(*this);
   Subscribe<MouseClickEvent>(*this);
}

UIRoot::~UIRoot()
{}

void UIRoot::SetBounds(const Bounded& bounds)
{
   // Remove any preexisting constraints
   mSolver.remove_constraints(mBoundConstraints);

   mBoundConstraints = {
      mFrame.width == bounds.GetWidth(),
      mFrame.height == bounds.GetHeight(),

      mFrame.left == bounds.GetX(),
      mFrame.bottom == bounds.GetY()
   };

   // Se the values immediately, so that they can be accessed
   mFrame.width.set_value(bounds.GetWidth());
   mFrame.height.set_value(bounds.GetHeight());
   mFrame.left.set_value(bounds.GetX());
   mFrame.bottom.set_value(bounds.GetY());

   // UIRoot covers the entirety of its bounds.
   mSolver.add_constraints(mBoundConstraints);
}

void UIRoot::AddConstraintsForElement(UIFrame& frame)
{
   mSolver.add_constraints({
      frame.centerX == (frame.left + frame.right)  / 2,
      frame.centerY == (frame.top  + frame.bottom) / 2,
         
      frame.width  == (frame.right  - frame.left),
      frame.height == (frame.top - frame.bottom),
      frame.width >= 0,
      frame.height >= 0,

      frame.z >= 0,
   });
}

void UIRoot::AddConstraint(const UIConstraint& constraintToAdd) {
   auto it = mConstraintMap.find(constraintToAdd.GetName());
   if (it != mConstraintMap.end()) {
      printf("Trying to add constraint with duped name: %s", constraintToAdd.GetName().c_str());
      assert(false && "Attempting to add 2 constraints with the same name");
      return;
   }
   
   mConstraintMap.insert(make_pair(constraintToAdd.GetName(), constraintToAdd));
   mSolver.add_constraint(constraintToAdd.GetInternalConstraint());
}
   
//
// Remove a constraint from the system.
//
void UIRoot::RemoveConstraint(std::string constraintNameToRemove) {
   auto it = mConstraintMap.find(constraintNameToRemove);
   
   if (it != mConstraintMap.end()) {
      mSolver.remove_constraint(it->second.GetInternalConstraint());
      mConstraintMap.erase(constraintNameToRemove);
   }
   else {
      printf("Trying to remove unknown constraint %s", constraintNameToRemove.c_str());
      assert(false && "Attempting to remove an unknown constraint");
   }
}

UIConstraint *UIRoot::GetConstraint(std::string constraintName)
{
   auto it = mConstraintMap.find(constraintName);
   if (it != mConstraintMap.end()) {
      return &it->second;
   }
   else {
      return NULL;
   }
}

void UIRoot::AddConstraints(const rhea::constraint_list& constraints)
{   
   mSolver.add_constraints(constraints);
}

void UIRoot::AddEditVar(const rhea::variable& variable)
{
   mSolver.add_edit_var(variable);
}

void UIRoot::Suggest(const rhea::variable& variable, double value)
{
   mSolver.suggest_value(variable, value);
   mDirty = true;
}

void UIRoot::Receive(const ElementAddedEvent& evt)
{
   AddConstraintsForElement(evt.element->GetFrame());
   mElements.push_back(evt.element);
}

void UIRoot::Receive(const ElementRemovedEvent& evt)
{
   // TODO how the heck we gonna do this?
}

void UIRoot::Receive(const MouseDownEvent& evt)
{
   // Make a shallow-copy of my elements, so that if the event
   // triggers additions/changes the iterator is not invalidated.
   std::vector<UIElement*> elements{mElements};
   for (UIElement* elem : elements)
   {
      if (elem->MouseDown(evt) == Handled)
      {
         return;
      }
   }
}

void UIRoot::Receive(const MouseMoveEvent& evt)
{
   // Make a shallow-copy of my elements, so that if the event
   // triggers additions/changes the iterator is not invalidated.
   std::vector<UIElement*> elements{mElements};
   for (UIElement* elem : elements)
   {
      if (elem->MouseMove(evt) == Handled)
      {
         return;
      }
   }
}

void UIRoot::Receive(const MouseUpEvent& evt)
{
   // Make a shallow-copy of my elements, so that if the event
   // triggers additions/changes the iterator is not invalidated.
   std::vector<UIElement*> elements{mElements};
   for (UIElement* elem : elements)
   {
      if (elem->MouseUp(evt) == Handled)
      {
         return;
      }
   }
}

void UIRoot::Receive(const MouseClickEvent& evt)
{
   // Make a shallow-copy of my elements, so that if the event
   // triggers additions/changes the iterator is not invalidated.
   std::vector<UIElement*> elements{mElements};
   for (UIElement* elem : elements)
   {
      if (elem->MouseClick(evt) == Handled)
      {
         return;
      }
   }
}

void UIRoot::UpdateRoot()
{
   mSolver.solve();

   if (mDirty)
   {
      mSolver.resolve();
      
      // TODO couple things:
      // 1. move this sort to another function,
      // 2. for aggregators that use indices, update them accordingly when we do swaps.
      std::function<bool(UIElement*,UIElement*)> GreaterThan = [](UIElement* lhs, UIElement* rhs) {
         return lhs->GetFrame().z.value() > rhs->GetFrame().z.value();
      };
      Shared::TimSortInPlace(mElements, GreaterThan);
      
      Emit<UIRebalancedEvent>();
      mDirty = false;
   }
}

void UIRoot::RenderRoot()
{
   for (auto& aggregator : mAggregators)
   {
      if (aggregator)
      {
         aggregator->Update();
         aggregator->Render();
      }
   }
}

}; // namespace Engine

}; // namespace CubeWorld

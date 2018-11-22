//
// UISystem - Provides the ability to render game UI components
//
// By Thomas Steinke
//

#pragma once

#include <vector>
#include <rhea/simplex_solver.hpp>

#include "../Aggregator/Aggregator.h"
#include "../Core/Input.h"
#include "../Event/EventManager.h"
#include "../Graphics/VBO.h"
#include "UIConstraint.h"
#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class UIRoot : public UIElement, public EventManager
{
public:
   UIRoot();
   ~UIRoot();

   //
   // Set the manager responsible for providing input to this UI.
   //
   void SetInput(const Input* input) { mInput = input; }

   //
   // Get the associated input manager.
   //
   const Input* GetInput() const { return mInput; }

   //
   // Create a constant set of size constraints for the entire UI
   //
   void SetBounds(const Bounded& bounds);
   
   //
   // Populate the simple constraints for a UIFrame.
   //
   void AddConstraintsForElement(UIFrame& frame);

   //
   // Add a UIContraint to our map.
   //
   void AddConstraint(const UIConstraint& constraintToAdd);
   
   //
   // Remove a UIConstraint from our map.
   //
   void RemoveConstraint(std::string constraintNameToRemove);

   //
   // Returns a pointer to a UIConstraint. Will return NULL if not found.
   //
   UIConstraint* GetConstraint(std::string constraintName);
   
   //
   // Add arbitrary contraints.
   //
   void AddConstraints(const rhea::constraint_list& constraints);

   //
   // Alias for rhea::simplex_solver::add_edit_var. Add a rhea::variable for editing
   //
   void AddEditVar(const rhea::variable& variable);

   //
   // Suggest a value to the solver
   //
   void Suggest(const rhea::variable& variable, double value);

   //
   // For debugging.
   //
   rhea::simplex_solver& GetSolver() { return mSolver; }

   //
   // Reserve a section of data in the appropriate UIAggregator.
   //
   template<typename Aggregator>
   typename Aggregator::Region Reserve(const size_t& numElements)
   {
      Aggregator* aggregator = GetAggregator<Aggregator>();
      return aggregator->Reserve(numElements);
   }

   //
   // Rebalance all elements according to contraints.
   //
   void UpdateRoot();

   //
   // Render the entire UI.
   //
   void RenderRoot();

   //
   // Receives an event whenever a new element is added.
   // This is where we set up simple constraints for that element's UIFrame.
   //
   virtual void Receive(const ElementAddedEvent& evt);
   virtual void Receive(const ElementRemovedEvent& evt);

   //
   // Mouse events are sent out from here.
   //
   virtual void Receive(const MouseDownEvent& evt);
   virtual void Receive(const MouseMoveEvent& evt);
   virtual void Receive(const MouseUpEvent& evt);
   virtual void Receive(const MouseClickEvent& evt);
   
protected:
   // Solves for the constraints we provide.
   rhea::simplex_solver mSolver;

   // The four constraints that bound this UI.
   rhea::constraint_list mBoundConstraints;

public:
   // Get an aggregator, and ensure it exists.
   template<typename Aggregator>
   Aggregator* GetAggregator()
   {
      const typename Aggregator::BaseAggregator::Family family = Aggregator::GetFamily();
      if (family >= mAggregators.size())
      {
         mAggregators.resize(family + 1);
      }

      Aggregator* ring = static_cast<Aggregator*>(mAggregators[family].get());
      if (ring == nullptr)
      {
         ring = new Aggregator();
         mAggregators[family].reset(ring);
      }
      return ring;
   }

private:
   // Input manager.
   const Input* mInput;

   // Aggregators for batch rendering.
   std::vector<std::unique_ptr<BaseAggregator>> mAggregators;
   
   // Holds all the elements in this tree, sorted by z-value.
   std::vector<UIElement*> mElements;

   // Keep an internal map of constraints that we'll use to allow constraint editing.
   std::map<std::string, UIConstraint> mConstraintMap;

   // Tracks whether something has rebalanced in the last frame.
   bool mDirty;
};
   
}; // namespace Engine

}; // namespace CubeWorld

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
#include "UIContextMenu.h"
#include "UIElement.h"
#include "UIRoot_ConstraintDebugging.h"

namespace CubeWorld
{

namespace Engine
{

class UIContextMenuParent; ///< Forward declare

class UIRoot : public UIElement, public EventManager
{

friend class UIRoot_ConstraintDebugging;

public:
   UIRoot(Input* input);
   ~UIRoot();

   //
   // Set the manager responsible for providing input to this UI.
   //
   void SetInput(Input* input) { mInput = input; }

   //
   // Get the associated input manager.
   //
   Input* GetInput() const { return mInput; }

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

   void RemoveConstraintsForElement(const UIElement* element);

   //
   // Returns a pointer to a UIConstraint. Will return NULL if not found.
   //
   UIConstraint* GetConstraint(std::string constraintName);
   
   //
   // Prints out all the constraints for an element and how they affect
   //    each axis
   //
   void DebugLogConstraintsForElement(const UIElement* element);

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
   // Spawn a new UIContextMenu at the given point
   //
   void CreateUIContextMenu(double x, double y, UIContextMenu::Choices choices);

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
   // Free a section of data in the appropriate UIAggregator.
   //
   template<typename Aggregator>
   void FreeRegion(typename Aggregator::Region regionToFree)
   {
      Aggregator* aggregator = GetAggregator<Aggregator>();
      aggregator->Free(regionToFree);
   }

   //
   // Rebalance all elements according to contraints.
   //
   virtual void UpdateRoot();

   //
   // Render the entire UI.
   //
   void RenderRoot();

   //
   // Receives an event whenever a new element is added.
   // This is where we set up simple constraints for that element's UIFrame.
   //
   virtual void Receive(const ElementAddedEvent& evt);

   virtual void ElementDestructing(UIElement* element);

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

   // All normal content. Calling Add() will add elements with this as the parent.
   UIElement* mContentLayer;

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
         ring = new Aggregator(this);
         mAggregators[family].reset(ring);
      }
      return ring;
   }

   UIElement* AddChild(std::unique_ptr<UIElement> &&element) override;

private:
   // Helper function that gives a shallow copy of just the active elements
   void GetActiveElements(const std::vector<UIElement*>& elementList, std::vector<UIElement*>* outElementList);
   
   // On ^D, turn on constraint debugging mode
   void ToggleDebugConstraints(int key, int action, int mods);

   // Input manager.
   Input* mInput;

   // Aggregators for batch rendering.
   std::vector<std::unique_ptr<BaseAggregator>> mAggregators;

   // Holds all the elements in this tree, sorted by z-value.
   std::vector<UIElement*> mElements;
   
   std::vector<UIGestureRecognizer*> mActiveRecognizers;

   // Keep an internal map of constraints that we'll use to allow constraint editing.
   std::map<std::string, UIConstraint> mConstraintMap;

   // The element, if any, that has captured the current click-and-drag of the mouse.
   UIElement* mActivelyCapturingElement;

   // UIElement that will capture mouse events if active and show you constraint information.
   UIRoot_ConstraintDebugging* mConstraintDebugger;
   
   // Parents any UIContextMenu we want to show. Lets us place it in front of all other content.
   UIContextMenuParent* mContextMenuLayer;

   std::unique_ptr<Input::KeyCallbackLink> mDebugKeycallback;

   // Tracks whether something has rebalanced in the last frame.
   bool mDirty;
};

}; // namespace Engine

}; // namespace CubeWorld

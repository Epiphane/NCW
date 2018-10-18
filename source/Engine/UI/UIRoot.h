//
// UISystem - Provides the ability to render game UI components
//
// By Thomas Steinke
//

#pragma once

#include <vector>
#include <rhea/simplex_solver.hpp>

#include "../Aggregator/Aggregator.h"
#include "../Event/EventManager.h"
#include "../Graphics/VBO.h"
#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class UIRoot : public UIElement, public EventManager
{
public:
   UIRoot(const Bounded& bounds);
   ~UIRoot();
   
   //
   // Populate the simple constraints for a UIFrame.
   //
   void AddConstraintsForElement(UIFrame& frame);

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
   void Receive(const ElementAddedEvent& evt);
   
protected:
   // Solves for the constraints we provide.
   rhea::simplex_solver mSolver;

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
   // Aggregators for batch rendering.
   std::vector<std::unique_ptr<BaseAggregator>> mAggregators;
   
   // Holds all the vertices pushed by this element's children.
   std::vector<Graphics::Font::CharacterVertexUV> mUIVertices;

   // VBO that ALL the UI elements will use.
   Engine::Graphics::VBO mRectanglesVBO;

   // Tracks whether something has rebalanced in the last frame.
   bool mDirty;
};
   
}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#include <Engine/Graphics/Program.h>

//#include <rhea/rhea/iostream.hpp> // Uncomment if you want to do something like `cout << rhea::variable`

#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{

UIRoot::UIRoot(const Bounded& bounds)
   : UIElement(this, nullptr)
   , mRectanglesVBO(Engine::Graphics::VBO::Vertices)
   , mDirty(false)
{
   // Disable autosolve, otherwise we try to solve whenever we add a new constraint
   mSolver.set_autosolve(false);
   mSolver.on_resolve = [&](rhea::simplex_solver&) {
      mDirty = true;
   };
   mSolver.on_variable_change = [&](const rhea::variable& variable, rhea::solver&) {
      // TODO maybe drill down and find the element that cares one day.
      mDirty = true;
   };

   // UIRoot covers the entirety of its bounds.
   mSolver.add_constraints({
      mFrame.width == bounds.GetWidth(),
      mFrame.height == bounds.GetHeight(),

      mFrame.left == bounds.GetX(),
      mFrame.bottom == bounds.GetY()
   });

   AddConstraintsForElement(mFrame);
   Subscribe<ElementAddedEvent>(*this);
}

UIRoot::~UIRoot()
{}

void UIRoot::AddConstraintsForElement(UIFrame& frame)
{
   mSolver.add_constraints({
      frame.centerX == (frame.left + frame.right)  / 2,
      frame.centerY == (frame.top  + frame.bottom) / 2,
         
      frame.width  == (frame.right  - frame.left),
      frame.height == (frame.top - frame.bottom),
      frame.width >= 0,
      frame.height >= 0,

      frame.z >= -1,
      frame.z <= 1,
   });
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
}

void UIRoot::UpdateRoot()
{
   mSolver.solve();

   if (mDirty)
   {
      mSolver.resolve();
      Emit<UIRebalancedEvent>();
      mDirty = false;
   }
}

void UIRoot::RenderRoot()
{
   glDepthFunc(GL_ALWAYS);
   
   for (auto& aggregator : mAggregators)
   {
      if (aggregator)
      {
         aggregator->Update();
         aggregator->Render();
      }
   }
   
   glDepthFunc(GL_LESS);
}

}; // namespace Engine

}; // namespace CubeWorld

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
{
   // Disable autosolve, otherwise we try to solve whenever we add a new constraint
   mSolver.set_autosolve(false);

   // UIRoot covers the entirety of its bounds.
   mSolver.add_constraints({
      mFrame.width == bounds.GetWidth(),
      mFrame.height == bounds.GetHeight(),

      mFrame.left == bounds.GetX(),
      mFrame.top == bounds.GetY()
   });

   AddConstraintsForElement(mFrame);
}

UIRoot::~UIRoot()
{}

void UIRoot::AddConstraintsForElement(UIFrame& frame)
{
   mSolver.add_constraints({
      frame.centerX == (frame.left + frame.right)  / 2,
      frame.centerY == (frame.top  + frame.bottom) / 2,
         
      frame.width  == (frame.right  - frame.left),
      frame.height == (frame.bottom - frame.top),
   });
}

void UIRoot::UpdateRoot()
{
   mSolver.solve();
}

void UIRoot::RenderRoot()
{
   // Add all the vertices from my children
   mUIVertices.clear();
   AddVertices(mUIVertices);
      
   mRectanglesVBO.BufferData(static_cast<GLsizei>(mUIVertices.size()) * sizeof(Graphics::Font::CharacterVertexUV), &mUIVertices[0], GL_STATIC_DRAW);
   Render(mRectanglesVBO, 0);
}

}; // namespace Engine

}; // namespace CubeWorld

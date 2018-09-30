// By Thomas Steinke

#include <GL/glew.h>

#include <Engine/Graphics/Program.h>

//#include <rhea/rhea/iostream.hpp> // Uncomment if you want to do something like `cout << rhea::variable`

#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{
   
   std::unique_ptr<UIRoot> sInstance = nullptr;
   
   UIRoot::UIRoot(Window* pWindow)
      : mRectanglesVBO(Engine::Graphics::VBO::Vertices)
   {
      // Disable autosolve, otherwise we try to solve whenever we add a new constraint
      mSolver.set_autosolve(false);
      
      mpRoot = this;
      
      // UIRoot's frame is at (0,0) and matches the window's size
      mSolver.add_constraints({
         mFrame.width  == pWindow->Width(),
         mFrame.height == pWindow->Height(),
         
         mFrame.left == 0,
         mFrame.top == 0
      });
      
      AddConstraintsForElement(*this);
   }
   
   
   UIRoot* UIRoot::Initialize(Window* pWindow)
   {
      sInstance = std::make_unique<UIRoot>(pWindow);
      return sInstance.get();
   }
   
   
   UIRoot* UIRoot::Instance()
   {
      assert(sInstance != nullptr);
      return sInstance.get();
   }
   
   
   void UIRoot::AddConstraintsForElement(UIElement& element) {
      UIFrame& frame = element.GetFrame();
      
      mSolver.add_constraints({
         frame.centerX == (frame.left + frame.right)  / 2,
         frame.centerY == (frame.top  + frame.bottom) / 2,
         
         frame.width  == (frame.right  - frame.left),
         frame.height == (frame.bottom - frame.top),
      });
   }

   
   void UIRoot::UpdateRoot() {
      mSolver.solve();
   }


   void UIRoot::RenderRoot() {
      // Add all the vertices from my children
      mUIVertices.clear();
      AddVertices(mUIVertices);
      
      mRectanglesVBO.BufferData(mUIVertices.size() * sizeof(Graphics::Font::CharacterVertexUV), &mUIVertices[0], GL_STATIC_DRAW);
      Render(mRectanglesVBO, 0);
   }
   
}; // namespace Engine

}; // namespace CubeWorld

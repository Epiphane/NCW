//
// UISystem - Provides the ability to render game UI components
//
// By Thomas Steinke
//

#pragma once

#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/Maybe.h>
#include <Engine/Core/Singleton.h>
#include <Engine/Graphics/VBO.h>

#include <rhea/simplex_solver.hpp>

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class UIRoot : public UIElement
{
public:
   UIRoot(const Bounded& bounds);
   ~UIRoot();
   
   //
   // Populate the simple constraints for a UIFrame.
   //
   void AddConstraintsForElement(UIFrame& frame);
   
   //
   // Rebalance all elements according to contraints.
   //
   void UpdateRoot();

   //
   // Render the entire UI.
   //
   void RenderRoot();
   
protected:
   // Solves for the constraints we provide.
   rhea::simplex_solver mSolver;
   
private:
   // Holds all the vertices pushed by this element's children.
   std::vector<Graphics::Font::CharacterVertexUV> mUIVertices;

   // VBO that ALL the UI elements will use.
   Engine::Graphics::VBO mRectanglesVBO;
};
   
}; // namespace Engine

}; // namespace CubeWorld

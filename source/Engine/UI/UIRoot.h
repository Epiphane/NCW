//
// UISystem - Provides the ability to render game UI components
//
// By Thomas Steinke
//

#pragma once

#include <vector>

#include <Engine/Core/Window.h>
#include <Engine/Graphics/VBO.h>

#include <rhea/simplex_solver.hpp>

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class UIRoot : public UIElement {
public:
   static UIRoot* Initialize(Window* window);
   static UIRoot* Instance();
   
   UIRoot(Window *pWindow);
   
   void AddConstraintsForElement(UIElement& element);    ///< New element. Populate the constraints for it.
   
   void UpdateRoot();
   void RenderRoot();
   
protected:
   rhea::simplex_solver  mSolver;         ///< Solves for the constraints we provide
   
private:
   std::vector<Graphics::Font::CharacterVertexUV> mUIVertices;   ///< Holds all the vertices pushed by this element's children.
   Engine::Graphics::VBO mRectanglesVBO;  ///< VBO that ALL the UI elements will use
};
   
}; // namespace Engine

}; // namespace CubeWorld

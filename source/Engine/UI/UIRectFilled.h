//
// UIRectFilled – render a solid rectangle with a given color in the UI.
//
// By Thomas Steinke + Elliot Fiske
//

#pragma once

#include <Engine/UI/UIElement.h>
#include <Engine/Graphics/VBO.h>


namespace CubeWorld
{

namespace Engine
{
   
   class UIRectFilled : public UIElement {
   public:
      UIRectFilled();
      
      virtual void AddVertices(std::vector<VertexData>& outVertices);
      virtual void Render(Engine::Graphics::VBO& vbo, size_t offset);
      
   private:
      static GLuint program;
      static GLuint aPosition;
      static GLuint uWindowSize, uColor;
   };

   
}; // namespace Engine

}; // namespace CubeWorld

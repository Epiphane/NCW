//
// UIRectFilled – render a solid rectangle with a given color in the UI.
//
// By Thomas Steinke
//

#pragma once

#include <Engine/UI/UIElement.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/Graphics/Program.h>


namespace CubeWorld
{

namespace Engine
{
   
   class UIRectFilled : public UIElement {
   public:
      UIRectFilled();
      
      virtual void AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices);
      virtual int  Render(Engine::Graphics::VBO& vbo, size_t offset);
      
      void SetColor(float r, float g, float b, float a);
      glm::vec4 GetColor();
      
   private:
      glm::vec4 mColor;
      
      static std::unique_ptr<Engine::Graphics::Program> program;
   };

   
}; // namespace Engine

}; // namespace CubeWorld

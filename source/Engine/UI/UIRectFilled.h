//
// UIRectFilled – render a solid rectangle with a given color in the UI.
//
// By Thomas Steinke
//

#pragma once

#include "UIElement.h"
#include <Engine/Graphics/Program.h>

namespace CubeWorld
{

namespace Engine
{
   
class UIRectFilled : public UIElement {
public:
   UIRectFilled(UIRoot* root, UIElement* parent);

   //
   // Add vertices for this rectangle to outVertices.
   //
   virtual void AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices) override;

   //
   // Makes the OpenGL calls to render a solid rectangle.
   //
   // `offset` is the current offset into the VBO we're rendering with.
   //    This method returns the NEW offset(in this case, 2 vertices
   //    past the previous offset)
   //
   virtual size_t Render(Engine::Graphics::VBO& vbo, size_t offset) override;

   //
   // Set the fill color of this rectangle. r, g, b, and a are between 0 and 1.
   //
   void SetColor(glm::vec4 color) { mColor = color; }

   //
   // Get the current fill color of this rectangle.
   //
   glm::vec4 GetColor() { return mColor; }

private:
   glm::vec4 mColor;

   static std::unique_ptr<Engine::Graphics::Program> program;
};

   
}; // namespace Engine

}; // namespace CubeWorld

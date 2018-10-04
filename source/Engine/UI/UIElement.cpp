//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke

#include <Engine/UI/UIRoot.h>

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

UIElement::UIElement(UIRoot* root, UIElement* parent)
   : mpRoot(root)
   , mpParent(parent)
{
}

void UIElement::AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices) {
   for (auto& child : mChildren) {
      child->AddVertices(outVertices);
   }
}

size_t UIElement::Render(Engine::Graphics::VBO& vbo, size_t offset) {
   for (auto& child : mChildren) {
      offset = child->Render(vbo, offset);
   }
      
   return offset;
}

void UIElement::AddConstraint(std::string nameKey, const rhea::constraint& constraint) {
   mConstraints[nameKey] = constraint;
      
}
   
}; // namespace Engine

}; // namespace CubeWorld

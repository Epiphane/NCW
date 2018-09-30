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
   UIElement::UIElement()
      : mpParent(nullptr)
   {
   }
   
   
   /**
    * Add an element as a child of this node.
    */
   void UIElement::AddChild(UIElement& newChild) {
      if (newChild.mpParent != nullptr)
         assert(false); // Child cannot have more than one parent!!
      
      newChild.mpParent = this;
      newChild.mpRoot = mpRoot;  // Tell the new child about our Lord and Saviour mpRoot
      mpChildren.push_back(&newChild);
      
      mpRoot->AddConstraintsForElement(newChild);
   }
   
   
   /**
    * Update all this node's children. Pass in the vertex data array to my children
    *  so they can add their vertices to it.
    */
   void UIElement::AddVertices(std::vector<VertexData>& outVertices) {
      for (int ndx = 0; ndx < mpChildren.size(); ndx++) {
         mpChildren[ndx]->AddVertices(outVertices);
      }
   }
   
   
   /**
    * Render all this node's children. Pass in the VBO to my children in case
    *    they want to render with it.
    */
   void UIElement::Render(Engine::Graphics::VBO& vbo, size_t offset) {
      for (int ndx = 0; ndx < mpChildren.size(); ndx++) {
         mpChildren[ndx]->Render(vbo, offset);
      }
   }
   
}; // namespace Engine

}; // namespace CubeWorld

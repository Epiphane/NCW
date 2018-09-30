//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke + Elliot Fiske
//

#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <Engine/Graphics/VBO.h>
#include <rhea/rhea/variable.hpp>

namespace CubeWorld
{

namespace Engine
{

   
/**
 * Constraint-based rectangle. Has several "innate" constraints,
 *  such as `bottom - top == height`. Used as the backbone for
 *  laying out UI elements.
 */
struct UIFrame {
   rhea::variable left, right, top, bottom;
   rhea::variable centerX, centerY, width, height;
};
   

/**
 * Data for one vertex. We build up an array of vertices by
 *  walking through the UI heirarchy.
 */
struct VertexData {
   VertexData() {
      std::memset(this, 0, sizeof(VertexData));
   };
   
   glm::vec2 position;//, uv;
//   glm::vec4 color;
};
   
class UIRoot; // Forward declare
   
class UIElement {
public:
   UIElement();
   
   UIFrame& GetFrame() { return mFrame; }
   
   virtual void AddChild(UIElement& newChild);
   
   virtual void AddVertices(std::vector<VertexData>& outVertices);
   virtual void Render(Engine::Graphics::VBO& vbo, size_t offset);
   
   UIFrame mFrame;         ///< Contains the coordinates and size of the element.
   
protected:
   UIRoot* mpRoot;         ///< The parent of all parents
   
   UIElement* mpParent;    ///< My parent in the UI heirarchy
   std::vector<UIElement*> mpChildren;
};
   
}; // namespace Engine

}; // namespace CubeWorld

//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke
//

#pragma once

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <Engine/Graphics/VBO.h>
#include <Engine/Graphics/FontManager.h>

#include <rhea/rhea/variable.hpp>
#include <rhea/rhea/constraint.hpp>

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
   
   virtual void AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices);
   virtual int  Render(Engine::Graphics::VBO& vbo, size_t offset);
   
   void AddConstraint(std::string nameKey, rhea::constraint constraint);
   
protected:
   UIFrame mFrame;         ///< Contains the coordinates and size of the element.
   
   std::map<std::string, rhea::constraint> mConstraints; ///< Map where key is the constraint's name and value is the constraint
   std::vector<UIElement*> mpChildren;
   
   UIRoot* mpRoot;         ///< The parent of all parents
   UIElement* mpParent;    ///< My parent in the UI heirarchy
};
   
}; // namespace Engine

}; // namespace CubeWorld

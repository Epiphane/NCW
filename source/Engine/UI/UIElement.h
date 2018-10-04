//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <Engine/Graphics/VBO.h>
#include <Engine/Graphics/FontManager.h>

//#include <rhea/rhea/variable.hpp>
#include <rhea/variable.hpp>
#include <rhea/constraint.hpp>

namespace CubeWorld
{

namespace Engine
{

/**
 * Constraint-based rectangle. Has several "innate" constraints,
 *  such as `bottom - top == height`. Used as the backbone for
 *  laying out UI elements.
 */
struct UIFrame
{
   rhea::variable left, right, top, bottom;
   rhea::variable centerX, centerY, width, height;
};

/**
 * Data for one vertex. We build up an array of vertices by
 *  walking through the UI heirarchy.
 */
struct VertexData
{
   VertexData() {
      std::memset(this, 0, sizeof(VertexData));
   };
   
   glm::vec2 position, uv;
//   glm::vec4 color; // Maybe someday
};

class UIRoot; // Forward declare

class UIElement
{
public:
   UIElement(UIRoot* root, UIElement* parent);

   //
   // UIFrame manipulation.
   //
   UIFrame& GetFrame() { return mFrame; }

   //
   // Add a new element of type E as a child of this one.
   // Returns a pointer to the element, for referencing, configuring, etc.
   //
   template <typename E, typename ...Args>
   E* AddChild(Args ...args)
   {
      std::unique_ptr<E> elem(new E(mpRoot, this, std::forward<Args>(args)...));
      E* result = elem.get();

      mChildren.push_back(std::move(elem));
      mpRoot->AddConstraintsForElement(result->GetFrame());

      return result;
   }

   //
   // Update all this node's children. Pass in the vertex data array to my children
   // so they can add their vertices to it.
   //
   virtual void AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices);

   //
   // Render all this node's children. Pass in the VBO to my children in case
   // they want to render with it.
   //
   virtual size_t Render(Engine::Graphics::VBO& vbo, size_t offset);

   //
   // Add a named constraint to this element. Element will report this to its mpRoot.
   //
   void AddConstraint(std::string nameKey, const rhea::constraint& constraint);

protected:
   UIFrame mFrame;         ///< Contains the coordinates and size of the element.

   std::map<std::string, rhea::constraint> mConstraints; ///< Map where key is the constraint's name and value is the constraint

   // Children are owned by their parent elements.
   std::vector<std::unique_ptr<UIElement>> mChildren;

   UIRoot* mpRoot;
   UIElement* mpParent;    ///< My parent in the UI heirarchy
};

}; // namespace Engine

}; // namespace CubeWorld

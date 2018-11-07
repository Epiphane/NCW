//
// UIConstrainable.h
//
// UIConstrainable – a wrapper around rhea::constraint that formalizes
//                   everything you would want to do when creating a UI
//
// By Elliot Fiske
//

#pragma once

#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "../Core/Bounded.h"
#include "UIConstraint.h"


namespace CubeWorld
{

namespace Engine
{
   
class UIRoot; ///< Forward declare
   
typedef std::vector<UIConstraint*> ConstraintArray;
   
/**
 * Constraint-based rectangle. Has several "innate" constraints,
 *  such as `bottom - top == height`. Used as the backbone for
 *  laying out UI elements.
 */
struct UIFrame : public Bounded
{  
   rhea::variable left, right, top, bottom;
   rhea::variable centerX, centerY, width, height;
   
   // Larger z is displayed on top
   rhea::variable z;
   
   glm::vec3 GetTopRight()
   {
      return glm::vec3(right.value(), top.value(), z.value());
   }
   
   glm::vec3 GetBottomLeft()
   {
      return glm::vec3(left.value(), bottom.value(), z.value());
   }
   
   rhea::linear_expression ConvertTargetToVariable(UIConstraint::Target target);
   
   uint32_t GetX() const override { return left.int_value(); }
   uint32_t GetY() const override { return bottom.int_value(); }
   uint32_t GetWidth() const override { return width.int_value(); }
   uint32_t GetHeight() const override { return height.int_value(); }
};
   
class UIConstrainable {
public:
   UIConstrainable(UIRoot* root);
   
   UIConstraint ConstrainToLeftOf  (UIConstrainable* other, double offset, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainToTopOf   (UIConstrainable* other, double offset, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainToRightOf (UIConstrainable* other, double offset, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainToBottomOf(UIConstrainable* other, double offset, UIConstraint::Options options = UIConstraint::Options());
   
   //
   // Set the name of this element
   //
   void SetName(const std::string& name) { mName = name; }
   
   //
   // Get the name of this element
   //
   std::string GetName() const { return mName; }
   
   //
   // UIFrame manipulation.
   //
   UIFrame& GetFrame() { return mFrame; }
   
protected:
   
   UIRoot *mpRoot;
   
   std::string mName;
   
   // Contains the coordinates and size of the element
   UIFrame mFrame;
};

}; // namespace Engine

}; // namespace CubeWorld

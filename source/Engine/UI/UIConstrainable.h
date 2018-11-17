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
   UIConstrainable(UIRoot* root, const std::string& name);
   
   // Constrain measurements to a CONSTANT
   UIConstraint ConstrainWidth(double width, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainHeight(double height, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainAspectRatio(double aspectRatio, UIConstraint::Options options = UIConstraint::Options());
   
   // Constrain yourself to ANOTHER ELEMENT
   UIConstraint ConstrainToLeftOf (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainAbove    (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainToRightOf(UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainBelow    (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   
   UIConstraint ConstrainWidthTo (UIConstrainable* other, double constant = 0.0, double multiplier = 1.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainHeightTo(UIConstrainable* other, double constant = 0.0, double multiplier = 1.0, UIConstraint::Options options = UIConstraint::Options());
   
   UIConstraint ConstrainLeftAlignedTo  (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainTopAlignedTo   (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainRightAlignedTo (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainBottomAlignedTo(UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   
   UIConstraint ConstrainHorizontalCenterTo(UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainVerticalCenterTo  (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   std::pair<UIConstraint, UIConstraint> ConstrainCenterTo          (UIConstrainable* other, double xOffset = 0.0, double yOffset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   
   void ConstrainEqualBounds(UIConstrainable* other, double leftMargin = 0.0, double topMargin = 0.0, double rightMargin = 0.0, double bottomMargin = 0.0, UIConstraint::Options options = UIConstraint::Options());
   
   UIConstraint ConstrainInFrontOf(UIConstrainable* other, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint ConstrainBehind   (UIConstrainable* other, UIConstraint::Options options = UIConstraint::Options());
   
   
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

   std::string mName;

   UIRoot *mpRoot;

   // Contains the coordinates and size of the element
   UIFrame mFrame;
};

}; // namespace Engine

}; // namespace CubeWorld

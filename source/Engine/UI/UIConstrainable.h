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

class UIRootDep; ///< Forward declare

/**
 * Constraint-based rectangle. Has several "innate" constraints,
 *  such as `bottom - top == height`. Used as the backbone for
 *  laying out UI elements.
 */
struct UIFrame : public Bounded
{
   rhea::variable left, right, top, bottom;

   // Larger z is displayed on top
   rhea::variable z;

   // What is the largest Z value amongst my children?
   //  Used to make a constraint like "In front of me AND all my children."
   rhea::variable biggestDescendantZ;

   glm::vec3 GetTopRight()
   {
      return glm::vec3(right.value(), top.value(), z.value());
   }

   glm::vec3 GetBottomLeft()
   {
      return glm::vec3(left.value(), bottom.value(), z.value());
   }

   uint32_t GetX() const override { return uint32_t(left.int_value()); }
   uint32_t GetY() const override { return uint32_t(bottom.int_value()); }
   uint32_t GetWidth() const override { return uint32_t(right.int_value() - left.int_value()); }
   uint32_t GetHeight() const override { return uint32_t(top.int_value() - bottom.int_value()); }
};

class UIConstrainable : public Bounded {
public:
   UIConstrainable(UIRootDep* root, const std::string& name);
   virtual ~UIConstrainable() = default;

   virtual rhea::linear_expression ConvertTargetToVariable(UIConstraint::Target target) const;

   // Constrain measurements to a CONSTANT
   UIConstraint& ConstrainWidth(double width, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainHeight(double height, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainAspectRatio(double aspectRatio, UIConstraint::Options options = UIConstraint::Options());

   // Constrain yourself to ANOTHER ELEMENT
   UIConstraint& ConstrainToLeftOf (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainAbove    (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainToRightOf(UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainBelow    (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());

   UIConstraint& ConstrainWidthTo (UIConstrainable* other, double constant = 0.0, double multiplier = 1.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainHeightTo(UIConstrainable* other, double constant = 0.0, double multiplier = 1.0, UIConstraint::Options options = UIConstraint::Options());
   void ConstrainDimensionsTo(UIConstrainable* other, double constant = 0.0, double multiplier = 1.0, UIConstraint::Options options = UIConstraint::Options());

   UIConstraint& ConstrainLeftAlignedTo  (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainTopAlignedTo   (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainRightAlignedTo (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainBottomAlignedTo(UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());

   UIConstraint& ConstrainHorizontalCenterTo(UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainVerticalCenterTo  (UIConstrainable* other, double offset = 0.0, UIConstraint::Options options = UIConstraint::Options());
   void ConstrainCenterTo(UIConstrainable* other, double xOffset = 0.0, double yOffset = 0.0, UIConstraint::Options options = UIConstraint::Options());

   void ConstrainEqualBounds(UIConstrainable* other, double leftMargin = 0.0, double topMargin = 0.0, double rightMargin = 0.0, double bottomMargin = 0.0, UIConstraint::Options options = UIConstraint::Options());

   UIConstraint& ConstrainInFrontOf(UIConstrainable* other, UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainBehind   (UIConstrainable* other, UIConstraint::Options options = UIConstraint::Options());

   UIConstraint& ConstrainInFrontOfAllDescendants(UIConstrainable* other, UIConstraint::Options options = UIConstraint::Options());

   UIConstraint& ConstrainWidthToContent(UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainHeightToContent(UIConstraint::Options options = UIConstraint::Options());
   UIConstraint& ConstrainAspectRatioToContent(UIConstraint::Options options = UIConstraint::Options());

   //
   // Set the name of this element
   //
   UIConstrainable* SetName(const std::string& name) { mName = name; return this; }

   //
   // Get the name of this element
   //
   std::string GetName() const { return mName; }

   //
   // UIFrame manipulation.
   //
   UIFrame& GetFrame() { return mFrame; }

   // Bounded implementation
   uint32_t GetX() const override { return mFrame.GetX(); }
   uint32_t GetY() const override { return mFrame.GetY(); }
   uint32_t GetWidth() const override { return mFrame.GetWidth(); }
   uint32_t GetHeight() const override { return mFrame.GetHeight(); }

protected:
   UIRootDep* mpRoot;

   std::string mName;

   // Contains the coordinates and size of the element
   UIFrame mFrame;

   // An ever-incrementing ID for this element. Used for anonymous elements
   static uint64_t sID;
};

}; // namespace Engine

}; // namespace CubeWorld

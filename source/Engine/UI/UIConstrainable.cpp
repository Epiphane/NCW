//
// UIConstrainable.cpp
//
// UIConstrainable – Describes a UI object you can constrain other UI objects to.
//                   Contains all the helper functions for making constraints.
//
// By Elliot Fiske
//

#include "UIConstrainable.h"

#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{

//
// Converts from our Target enum to a rhea::linear_expression.
//
rhea::linear_expression UIFrame::ConvertTargetToVariable(UIConstraint::Target target)
{
   std::map<UIConstraint::Target, rhea::linear_expression> mapping = {
      {UIConstraint::Left,   left},
      {UIConstraint::Top,    top},
      {UIConstraint::Bottom, bottom},
      {UIConstraint::Right,  right},
      
      {UIConstraint::CenterX, (left + right) / 2 },
      {UIConstraint::CenterY, (top + bottom) / 2 },
      
      {UIConstraint::Width,   (right - left) },
      {UIConstraint::Height,  (top - bottom) },
   };
   
   return mapping[target];
}
   

UIConstrainable::UIConstrainable(UIRoot* root)
   : mpRoot(root)
{
}

/* Constrain constant width:
 *
 *  |   ME  |
 *  | width |
 */
UIConstraint UIConstrainable::ConstrainWidth(double width, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_constantWidth";
   
   options.mConstant = width;
   
   UIConstraint newConstraint(this, NULL, UIConstraint::Width, UIConstraint::NoTarget, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/* Constrain constant height:
 *  __ __
 *  ME ^
 *     height
 *     v
 *  __ __
 */
UIConstraint UIConstrainable::ConstrainHeight(double height, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_constantHeight";
   
   options.mConstant = height;
   
   UIConstraint newConstraint(this, NULL, UIConstraint::Height, UIConstraint::NoTarget, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/* Constrain aspect ratio:
 * 
 * Aspect ratio = WIDTH / HEIGHT
 */
UIConstraint UIConstrainable::ConstrainAspectRatio(double aspectRatio, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_constantAspectRatio";
   
   options.mMultiplier = aspectRatio;
   
   UIConstraint newConstraint(this, this, UIConstraint::Width, UIConstraint::Height, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/* Constrain To Left Of:
 *
 *  ME -offset- <FIRST ARGUMENT>  
 */
UIConstraint UIConstrainable::ConstrainToLeftOf(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_leftOf_";
   
   options.mConstant = -offset;   // Flipped!
   
   UIConstraint newConstraint(this, other, UIConstraint::Right, UIConstraint::Left, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/* Constrain To Top Of:
 *
 *      ME
 *       ^ 
 *     offset
 *       v
 * <FIRST ARGUMENT>  
 */
UIConstraint UIConstrainable::ConstrainAbove(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_above_";
   
   options.mConstant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Bottom, UIConstraint::Top, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/* Constrain To Right Of:
 *
 *  <FIRST ARGUMENT>  -offset-  ME
 */
UIConstraint UIConstrainable::ConstrainToRightOf(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_rightOf";
   
   options.mConstant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Left, UIConstraint::Right, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
   
/* Constrain To Bottom Of:
 *
 * <FIRST ARGUMENT>
 *       ^ 
 *     offset
 *       v
 *      ME
 */
UIConstraint UIConstrainable::ConstrainBelow(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_below_";
   
   options.mConstant = -offset;   // Flipped!
   
   UIConstraint newConstraint(this, other, UIConstraint::Top, UIConstraint::Bottom, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/* Constrain Widths:
 *
 * |<FIRST ARG>|
 * |     ME     + offset|
 */
UIConstraint UIConstrainable::ConstrainWidthTo(UIConstrainable* other, double constant, double multiplier, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_widthTo_";
   
   options.mConstant = constant;
   options.mMultiplier = multiplier;
   
   UIConstraint newConstraint(this, other, UIConstraint::Width, UIConstraint::Width, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/* Constrain Heights:
 * ___________       ___
 * <FIRST ARG>
 *                    ME
 * ___________   
 *                 + offset
 *                   ___                     
 */
UIConstraint UIConstrainable::ConstrainHeightTo(UIConstrainable* other, double constant, double multiplier, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_heightTo_";
   
   options.mConstant = constant;
   options.mMultiplier = multiplier;
   
   UIConstraint newConstraint(this, other, UIConstraint::Height, UIConstraint::Height, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/*
 * Constrain left edges to one another
 */
UIConstraint UIConstrainable::ConstrainLeftAlignedTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_alignLeftWith_";
   
   options.mConstant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Left, UIConstraint::Left, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/*
 * Constrain top edges to one another
 */
UIConstraint UIConstrainable::ConstrainTopAlignedTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_alignTopWith_";
   
   options.mConstant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Top, UIConstraint::Top, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/*
 * Constrain right edges to one another
 */
UIConstraint UIConstrainable::ConstrainRightAlignedTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_alignRightWith_";
   
   options.mConstant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Right, UIConstraint::Right, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/*
 * Constrain bottom edges to one another
 */
UIConstraint UIConstrainable::ConstrainBottomAlignedTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_alignBottomWith_";
   
   options.mConstant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Bottom, UIConstraint::Bottom, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/*
 * Constrain horizontal center values to one another
 */
UIConstraint UIConstrainable::ConstrainHorizontalCenterTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_alignHorizontalCenterTo_";
   
   options.mConstant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::CenterX, UIConstraint::CenterX, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/*
 * Constrain vertical center values to one another
 */
UIConstraint UIConstrainable::ConstrainVerticalCenterTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_alignVerticalCenterTo_";
   
   options.mConstant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::CenterY, UIConstraint::CenterY, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/*
 * Constrain center points to one another
 */
std::pair<UIConstraint, UIConstraint> UIConstrainable::ConstrainCenterTo(UIConstrainable* other, double xOffset, double yOffset, UIConstraint::Options options) {
   UIConstraint verticalConstraint  = ConstrainVerticalCenterTo  (other, yOffset, options);
   UIConstraint horizontalContraint = ConstrainHorizontalCenterTo(other, xOffset, options);
   
   return std::make_pair(horizontalContraint, verticalConstraint);
}

   

   
}; // namespace Engine

}; // namespace CubeWorld

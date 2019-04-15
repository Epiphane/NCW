//
// UIConstrainable.cpp
//
// UIConstrainable – Describes a UI object you can constrain other UI objects to.
//                   Contains all the helper functions for making constraints.
//
// By Elliot Fiske
//

#include <RGBText/Format.h>

#include "UIConstrainable.h"

#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{

uint64_t UIConstrainable::sID = 0;

UIConstrainable::UIConstrainable(UIRoot* root, const std::string& name)
   : mpRoot(root)
   , mName(name.empty() ? Format::FormatString("Element_%1", sID++) : name)
{
}
   
//
// Converts from our Target enum to a rhea::linear_expression.
//
rhea::linear_expression UIConstrainable::ConvertTargetToVariable(UIConstraint::Target target) const
{
   std::map<UIConstraint::Target, rhea::linear_expression> mapping = {
      {UIConstraint::Left,   mFrame.left},
      {UIConstraint::Top,    mFrame.top},
      {UIConstraint::Bottom, mFrame.bottom},
      {UIConstraint::Right,  mFrame.right},
      
      {UIConstraint::CenterX, (mFrame.left + mFrame.right) / 2 },
      {UIConstraint::CenterY, (mFrame.top + mFrame.bottom) / 2 },
      
      {UIConstraint::Width,   (mFrame.right - mFrame.left) },
      {UIConstraint::Height,  (mFrame.top - mFrame.bottom) },
      
      {UIConstraint::ZHeight, mFrame.z},
      {UIConstraint::ZHeightDescendants, mFrame.biggestDescendantZ}
   };
   
   assert(mapping.find(target) != mapping.end() && "Unknown constraint target for UIElement!");
   
   return mapping[target];
}

/* Constrain constant width:
 *
 *  |   ME  |
 *  | width |
 */
UIConstraint UIConstrainable::ConstrainWidth(double width, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_constantWidth";
   
   options.constant = width;
   
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
   if (options.customNameConnector == "")
      options.customNameConnector = "_constantHeight";
   
   options.constant = height;
   
   UIConstraint newConstraint(this, NULL, UIConstraint::Height, UIConstraint::NoTarget, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/* Constrain aspect ratio:
 * 
 * Aspect ratio = WIDTH / HEIGHT
 */
UIConstraint UIConstrainable::ConstrainAspectRatio(double aspectRatio, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_constantAspectRatio";
   
   options.multiplier = aspectRatio;
   
   UIConstraint newConstraint(this, this, UIConstraint::Width, UIConstraint::Height, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/* Constrain To Left Of:
 *
 *  ME -offset- <FIRST ARGUMENT>  
 */
UIConstraint UIConstrainable::ConstrainToLeftOf(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_leftOf_";
   
   options.constant = -offset;   // Flipped!
   
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
   if (options.customNameConnector == "")
      options.customNameConnector = "_above_";
   
   options.constant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Bottom, UIConstraint::Top, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/* Constrain To Right Of:
 *
 *  <FIRST ARGUMENT>  -offset-  ME
 */
UIConstraint UIConstrainable::ConstrainToRightOf(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_rightOf_";
   
   options.constant = offset;
   
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
   if (options.customNameConnector == "")
      options.customNameConnector = "_below_";
   
   options.constant = -offset;   // Flipped!
   
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
   if (options.customNameConnector == "")
      options.customNameConnector = "_widthTo_";
   
   options.constant = constant;
   options.multiplier = multiplier;
   
   UIConstraint newConstraint(this, other, UIConstraint::Width, UIConstraint::Width, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/* Constrain Heights:
 * ___________       ___
 *
 * <FIRST ARG>        ME
 *
 * ___________   
 *                 + offset
 *                   ___                     
 */
UIConstraint UIConstrainable::ConstrainHeightTo(UIConstrainable* other, double constant, double multiplier, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_heightTo_";
   
   options.constant = constant;
   options.multiplier = multiplier;
   
   UIConstraint newConstraint(this, other, UIConstraint::Height, UIConstraint::Height, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/* Constrain Widths and Heights:
* ___________       ___
* <FIRST ARG>
*                    ME + offset|
* ___________
*                   ___
*/
std::pair<UIConstraint, UIConstraint> UIConstrainable::ConstrainDimensionsTo(UIConstrainable* other, double constant, double multiplier, UIConstraint::Options options) {
   return std::make_pair(ConstrainHeightTo(other, constant, multiplier, options), ConstrainWidthTo(other, constant, multiplier, options));
}

/*
 * Constrain left edges to one another
 */
UIConstraint UIConstrainable::ConstrainLeftAlignedTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_alignLeftWith_";
   
   options.constant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Left, UIConstraint::Left, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/*
 * Constrain top edges to one another
 */
UIConstraint UIConstrainable::ConstrainTopAlignedTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_alignTopWith_";
   
   options.constant = -offset; // Flipped!
   
   UIConstraint newConstraint(this, other, UIConstraint::Top, UIConstraint::Top, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
/*
 * Constrain right edges to one another
 */
UIConstraint UIConstrainable::ConstrainRightAlignedTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_alignRightWith_";
   
   options.constant = -offset; // Flipped!
   
   UIConstraint newConstraint(this, other, UIConstraint::Right, UIConstraint::Right, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/*
 * Constrain bottom edges to one another
 */
UIConstraint UIConstrainable::ConstrainBottomAlignedTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_alignBottomWith_";
   
   options.constant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::Bottom, UIConstraint::Bottom, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/*
 * Constrain horizontal center values to one another
 */
UIConstraint UIConstrainable::ConstrainHorizontalCenterTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_alignHorizontalCenterTo_";
   
   options.constant = offset;
   
   UIConstraint newConstraint(this, other, UIConstraint::CenterX, UIConstraint::CenterX, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/*
 * Constrain vertical center values to one another
 */
UIConstraint UIConstrainable::ConstrainVerticalCenterTo(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_alignVerticalCenterTo_";
   
   options.constant = offset;
   
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

/**
 * Constrain bounds equal, with an optional margin on each side
 */
std::tuple<UIConstraint, UIConstraint, UIConstraint, UIConstraint> UIConstrainable::ConstrainEqualBounds(UIConstrainable* other, double leftMargin, double topMargin, double rightMargin, double bottomMargin, UIConstraint::Options options) {
   UIConstraint leftConstraint   = ConstrainLeftAlignedTo  (other, leftMargin,   options);
   UIConstraint topConstraint    = ConstrainTopAlignedTo   (other, topMargin,    options);
   UIConstraint rightConstraint  = ConstrainRightAlignedTo (other, rightMargin,  options);
   UIConstraint bottomConstraint = ConstrainBottomAlignedTo(other, bottomMargin, options);
   
   return std::make_tuple(leftConstraint, topConstraint, rightConstraint, bottomConstraint);
}
   
/* Constrain In Front Of:
 *
 * Constraints ME to have a HIGHER Z value than 'other', thus putting me in front.
 */
UIConstraint UIConstrainable::ConstrainInFrontOf(UIConstrainable* other, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_inFrontOf_";
   
   options.relationship = UIConstraint::GreaterThanOrEqual;
   options.constant = 1.0f;

   UIConstraint newConstraint(this, other, UIConstraint::ZHeight, UIConstraint::ZHeight, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}

/* Constrain Behind:
 *
 * Constraints ME to have a LOWER Z value than 'other', thus putting me in back.
 */
UIConstraint UIConstrainable::ConstrainBehind(UIConstrainable* other, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_behind_";
   
   options.relationship = UIConstraint::LessThanOrEqual;
   options.constant = -1.0f;

   UIConstraint newConstraint(this, other, UIConstraint::ZHeight, UIConstraint::ZHeight, options);
   mpRoot->AddConstraint(newConstraint);

   return newConstraint;
}

/* Constrain In Front Of All Descendants
 *
 * Constraints ME to have a HIGHER Z value than 'other', AND all of its children.
 */
UIConstraint UIConstrainable::ConstrainInFrontOfAllDescendants(UIConstrainable* other, UIConstraint::Options options) {
   if (options.customNameConnector == "")
      options.customNameConnector = "_inFrontOfAllDescendants_";

   options.relationship = UIConstraint::GreaterThanOrEqual;
   options.constant = 1.0f;

   UIConstraint newConstraint(this, other, UIConstraint::ZHeight, UIConstraint::ZHeightDescendants, options);
   mpRoot->AddConstraint(newConstraint);

   return newConstraint;
}

/**
 * Constrain this element to use the width of its content as its layout width. 
 */
UIConstraint UIConstrainable::ConstrainWidthToContent(UIConstraint::Options options)
{
   if (options.customNameConnector == "")
      options.customNameConnector = "_widthToContentWidth";

   UIConstraint newConstraint(this, this, UIConstraint::Width, UIConstraint::ContentWidth, options);
   mpRoot->AddConstraint(newConstraint);

   return newConstraint;
}

/**
 * Constrain this element to use the height of its content as its layout height. 
 */
UIConstraint UIConstrainable::ConstrainHeightToContent(UIConstraint::Options options)
{
   if (options.customNameConnector == "")
      options.customNameConnector = "_heightToContentHeight";

   UIConstraint newConstraint(this, this, UIConstraint::Height, UIConstraint::ContentHeight, options);
   mpRoot->AddConstraint(newConstraint);

   return newConstraint;
}

/**
 * Constrain this element such that its (layout width) / (layout height) = (aspect ratio of its content)
 *
 * NOTE: We can't use an edit variable, because otherwise it wouldn't be a linear constraint.
 *          Therefore it's impossible to do this until we write our own simplex solver :P
 */
UIConstraint UIConstrainable::ConstrainAspectRatioToContent(UIConstraint::Options options)
{
   assert(false && "This isn't supported quite yet ;P");
   UIConstraint newConstraint(this, this, UIConstraint::Width, UIConstraint::Height, options);
   return newConstraint;
}

}; // namespace Engine

}; // namespace CubeWorld

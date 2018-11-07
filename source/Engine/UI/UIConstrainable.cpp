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
UIConstraint UIConstrainable::ConstrainToTopOf(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_onTopOf_";
   
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
      options.mCustomNameConnector = "_onBottomOf_";
   
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
UIConstraint UIConstrainable::ConstrainToBottomOf(UIConstrainable* other, double offset, UIConstraint::Options options) {
   if (options.mCustomNameConnector == "")
      options.mCustomNameConnector = "_onBottomOf_";
   
   options.mConstant = -offset;   // Flipped!
   
   UIConstraint newConstraint(this, other, UIConstraint::Top, UIConstraint::Bottom, options);
   mpRoot->AddConstraint(newConstraint);
   
   return newConstraint;
}
   
}; // namespace Engine

}; // namespace CubeWorld

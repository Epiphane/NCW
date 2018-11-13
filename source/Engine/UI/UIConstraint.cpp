//
// UIConstraint.cpp
//
// UIConstraint – a wrapper around rhea::constraint that formalizes
//                   everything you would want to do when creating a UI
//
// By Elliot Fiske
//

#include "UIConstraint.h"

#include "UIConstrainable.h"

namespace CubeWorld
{

namespace Engine
{
   
//
// Set the default values for the UIConstraint options
//
UIConstraint::Options::Options() {
   mCustomNameConnector = "";
   mPriority = BaseConstraint::REQUIRED_PRIORITY;
   
   mConstant = 0.0;
   mMultiplier = 1.0;
   
   mbIsConstantEditable   = false;
   mbIsMultiplierEditable = false;
   
   
}
   
UIConstraint::UIConstraint(UIConstrainable* primaryElement, UIConstrainable* secondaryElement,
                           Target primaryTarget, Target secondaryTarget, const Options& options) 
   : BaseConstraint(primaryElement->GetName() + 
                    options.mCustomNameConnector + 
                    (secondaryElement ? secondaryElement->GetName() : ""), options.mPriority)
   , mOptions(options)
   , mPrimaryElement  (primaryElement)
   , mSecondaryElement(secondaryElement)
   , mPrimaryTarget  (primaryTarget)
   , mSecondaryTarget(secondaryTarget)
{
   rhea::linear_expression leftSide = primaryElement->GetFrame().ConvertTargetToVariable(primaryTarget);
   rhea::linear_expression rightSide;
   
   if (secondaryElement != NULL)
      rightSide = secondaryElement->GetFrame().ConvertTargetToVariable(secondaryTarget);
   else
      rightSide = 0;
   
   switch (options.mRelationship) {
      case BaseConstraint::Equal:
         mInternalConstraint = (leftSide == rightSide * options.mMultiplier + options.mConstant);
         break;
      case BaseConstraint::GreaterOrEqual:
         mInternalConstraint = (leftSide >= rightSide * options.mMultiplier + options.mConstant);
         break;
      case BaseConstraint::LessThanOrEqual:
         mInternalConstraint = (leftSide <= rightSide * options.mMultiplier + options.mConstant);
         break;
      default:
         assert(false && "Invalid mRelationship value");
         break;
   }
   
   // TODO-EF: if mbIsConstantEditable is true, set up the constant as an edit variable instead.
   // TODO-EF: if mbIsMultiplierEditable is true, set up the multiplier as an edit variable instead.
}
   
}; // namespace Engine

}; // namespace CubeWorld

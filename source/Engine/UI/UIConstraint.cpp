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
   
static const std::map<UIConstraint::Target, std::string> CONSTRAINT_NAME_MAPPING = {
   {UIConstraint::Left,     "Left"},
   {UIConstraint::Bottom,   "Bottom"},
   {UIConstraint::Top,      "Top"},
   {UIConstraint::Right,    "Right"},
   {UIConstraint::Width,    "Width"},
   {UIConstraint::Height,   "Height"},
   {UIConstraint::CenterX,  "CenterX"},
   {UIConstraint::CenterY,  "CenterY"},
   {UIConstraint::ZHeight,  "ZHeight"},
   {UIConstraint::NoTarget, "NoTarget"},
};
   
std::string UIConstraint::StringFromConstraintTarget(Target target) 
{
   if (CONSTRAINT_NAME_MAPPING.find(target) == CONSTRAINT_NAME_MAPPING.end()) {
      assert(false && "Unknown constraint target name");
      return "NoTarget";
   }
   
   return CONSTRAINT_NAME_MAPPING.at(target);
}
   
UIConstraint::Target UIConstraint::ConstraintTargetFromString(std::string name)
{
   for (auto const& [target, mapName] : CONSTRAINT_NAME_MAPPING) {
      if (name == mapName) {
         return target;
      }
   }
   
   assert(false && "Unknown constraint target name");
   return NoTarget;
}

UIConstraint::UIConstraint(UIConstrainable* primaryElement, UIConstrainable* secondaryElement,
                           Target primaryTarget, Target secondaryTarget, const Options& options)
   : BaseConstraint(
      primaryElement->GetName() + options.customNameConnector + (secondaryElement ? secondaryElement->GetName() : ""),
      options.priority
   )
   , mOptions(options)
   , mPrimaryElement  (primaryElement)
   , mSecondaryElement(secondaryElement)
   , mPrimaryTarget  (primaryTarget)
   , mSecondaryTarget(secondaryTarget)
{
   rhea::linear_expression leftSide = primaryElement->GetFrame().ConvertTargetToVariable(primaryTarget);
   rhea::linear_expression rightSide;

   if (secondaryElement != nullptr)
      rightSide = secondaryElement->GetFrame().ConvertTargetToVariable(secondaryTarget);
   else
      rightSide = 0;

   switch (options.relationship) {
      case BaseConstraint::Equal:
         SetInternalConstraint(leftSide == rightSide * options.multiplier + options.constant);
         break;
      case BaseConstraint::GreaterOrEqual:
         SetInternalConstraint(leftSide >= rightSide * options.multiplier + options.constant);
         break;
      case BaseConstraint::LessThanOrEqual:
         SetInternalConstraint(leftSide <= rightSide * options.multiplier + options.constant);
         break;
      default:
         assert(false && "Invalid mRelationship value");
         break;
   }

   // TODO-EF: if mbIsConstantEditable is true, set up the constant as an edit variable instead.
   // TODO-EF: if mbIsMultiplierEditable is true, set up the multiplier as an edit variable instead.
}

UIConstraint::UIConstraint()
   : BaseConstraint("", REQUIRED_PRIORITY)
{
   mOptions = Options();
   mPrimaryElement   = NULL;
   mSecondaryElement = NULL;
   mPrimaryTarget   = NoTarget;
   mSecondaryTarget = NoTarget;
}

UIConstrainable* UIConstraint::GetPrimaryElement() const
{
   return mPrimaryElement;
}
   
UIConstraint::Target UIConstraint::GetPrimaryTarget() const
{
   return mPrimaryTarget;
}

UIConstrainable* UIConstraint::GetSecondaryElement() const
{
   return mSecondaryElement;
}
   
UIConstraint::Target UIConstraint::GetSecondaryTarget() const
{
   return mSecondaryTarget;
}
   
const UIConstraint::Options& UIConstraint::GetOptions() const
{
   return mOptions;
}

}; // namespace Engine

}; // namespace CubeWorld

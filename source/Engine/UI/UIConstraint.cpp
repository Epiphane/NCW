//
// UIConstraint.cpp
//
// UIConstraint – a wrapper around rhea::constraint that formalizes
//                   everything you would want to do when creating a UI
//
// By Elliot Fiske
//

#include "UIConstraint.h"

namespace CubeWorld
{

namespace Engine
{
   
//
// Set the default values for the UIConstraint options
//
UIConstraint::Options::Options() {
    mPriority = BaseConstraint::REQUIRED_PRIORITY;
    mbIsConstantEditable = false;
    mbIsMultiplierEditable = false;
}
   
//
// Copy constructor
//
UIConstraint::Options::Options(const UIConstraint::Options& otherOptions) {
   mPriority = otherOptions.mPriority;
   mbIsConstantEditable   = otherOptions.mbIsConstantEditable;
   mbIsMultiplierEditable = otherOptions.mbIsMultiplierEditable;
}
   
UIConstraint::UIConstraint(UIElement* primaryElement, UIElement* secondaryElement,
                           Target primaryTarget, Target secondaryTarget, Options options) 
   : BaseConstraint()
   , mOptions(options)
   , mPrimaryElement(primaryElement)
   , mSecondaryElement(secondaryElement)
   , mPrimaryTarget(primaryTarget)
   , mSecondaryTarget(secondaryTarget)
{
}
    

}; // namespace Engine

}; // namespace CubeWorld

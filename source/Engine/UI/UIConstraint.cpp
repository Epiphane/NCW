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
    mbIsConstantEditable   = false;
    mbIsMultiplierEditable = false;
}
   
UIConstraint::UIConstraint(UIElement* primaryElement, UIElement* secondaryElement,
                           Target primaryTarget, Target secondaryTarget, Options options) 
   : BaseConstraint(primaryElement->GetName() + options.mCustomNameConnector + secondaryElement->GetName(), options.mPriority)
   , mOptions(options)
   , mPrimaryElement  (primaryElement)
   , mSecondaryElement(secondaryElement)
   , mPrimaryTarget  (primaryTarget)
   , mSecondaryTarget(secondaryTarget)
{
}
    
void UIConstraint::SetOptions(Options newOptions) {
   mOptions = newOptions;
   
   SetPriority(newOptions.mPriority);
   
   
   SetDirty(true);
}
   
}; // namespace Engine

}; // namespace CubeWorld

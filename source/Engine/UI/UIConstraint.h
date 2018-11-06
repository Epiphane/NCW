//
// UIConstraint.cpp
//
// UIConstraint – a wrapper around rhea::constraint that formalizes
//                   everything you would want to do when creating a UI
//
// By Elliot Fiske
//

#pragma once

#include "BaseConstraint.h"
#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

//
// UI Constraint. Wraps a rhea::constraint with UI-specific fields
//    and helpers.
//
class UIConstraint : public BaseConstraint {
public:
   //
   // Lets you specify the constraint priority and whether it is an editable constraint
   //
   struct Options {
      Options();
      Options(const Options& otherOptions);
      
      double mPriority;
      bool mbIsConstantEditable;
      bool mbIsMultiplierEditable;
   };
   
   //
   // What aspect of the UI element are you constraining?
   //
   enum Target {
      Top, Left, Bottom, Right,
      Width, Height,
      
      CenterX, CenterY,
      
      ZHeight,
   };
    
   UIConstraint(UIElement* primaryElement, UIElement* secondaryElement, Target primaryTarget, Target secondaryTarget, Options options = Options());
   
private:
   Options mOptions;
   
   UIElement* mPrimaryElement;    ///< First element being constrained.
   UIElement* mSecondaryElement;  ///< Second element being constrained. NOTE: CAN BE THE SAME AS THE PRIMARY ELEMENT.
   
   Target mPrimaryTarget;         ///< Describes which aspect of each element is being constrained.
   Target mSecondaryTarget;
};

}; // namespace Engine

}; // namespace CubeWorld

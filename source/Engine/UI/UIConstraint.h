//
// UIConstraint.h
//
// UIConstraint – a wrapper around rhea::constraint that formalizes
//                   everything you would want to do when creating a UI
//
// By Elliot Fiske
//

#pragma once

#include "BaseConstraint.h"

namespace CubeWorld
{

namespace Engine
{
   
class UIConstrainable;  ///< Forward declare

//
// UI Constraint. Wraps a rhea::constraint with UI-specific fields
//    and helpers.
//
class UIConstraint : public BaseConstraint {
public:
   //
   // Lets you specify the constraint priority and whether it is an editable constraint. Also can give a custom name.
   //
   struct Options {
      Options();
      
      std::string mCustomNameConnector = ""; ///< The base constraint name will become "<primaryElement's name> + connector + <secondaryElement's name>"
      
      double mConstant = 0.0;                ///< Lets you set a scalar offset for the constraint.
                                             ///<  e.g. if mConstant = 10, the constraint might be (left == right + 10)
      
      double mMultiplier = 1.0;              ///< Lets you set a scalar multiplier for the constraint.
                                             ///<  e.g. if mMultiplier = 0.5, the constraint might be (width == height * 0.5)
      
      double mPriority = REQUIRED_PRIORITY;
      bool mbIsConstantEditable = false;     ///< If true, the 'constant' aspect of the constraint will be an edit_variable.   TODO-EF: Actually implement this
      bool mbIsMultiplierEditable = false;   ///< If true, the 'multiplier' aspect of the constraint will be an edit_variable. TODO-EF: Actually implement this
      
      BaseConstraint::Relationship mRelationship = BaseConstraint::Equal;  ///< Lets you specify ==, >= or <=
   };
   
   //
   // What aspect of the UI element are you constraining?
   //
   enum Target {
      Top, Left, Bottom, Right,
      Width, Height,
      
      CenterX, CenterY,
      
      ZHeight,
      ZHeightDescendants,
       
      NoTarget,
   };

   UIConstraint();
   UIConstraint(UIConstrainable* primaryElement, UIConstrainable* secondaryElement, Target primaryTarget, Target secondaryTarget, const Options& options = Options());
   
private:
   Options mOptions;
   
   UIConstrainable* mPrimaryElement;    ///< First element being constrained.
   UIConstrainable* mSecondaryElement;  ///< Second element being constrained. NOTE: CAN BE THE SAME AS THE PRIMARY ELEMENT.
   
   Target mPrimaryTarget;         ///< Describes which aspect of each element is being constrained.
   Target mSecondaryTarget;
};

}; // namespace Engine

}; // namespace CubeWorld

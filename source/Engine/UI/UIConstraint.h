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
      // https://stackoverflow.com/questions/43819314/default-member-initializer-needed-within-definition-of-enclosing-class-outside
      Options() noexcept {} // = default;

      BaseConstraint::Relationship relationship = BaseConstraint::Equal;  ///< Lets you specify ==, >= or <=
      std::string customNameConnector = ""; ///< The base constraint name will become "<primaryElement's name> + connector + <secondaryElement's name>"

      double constant = 0.0;                ///< Lets you set a scalar offset for the constraint.
                                            ///<  e.g. if mConstant = 10, the constraint might be (left == right + 10)

      double multiplier = 1.0;              ///< Lets you set a scalar multiplier for the constraint.
                                            ///<  e.g. if mMultiplier = 0.5, the constraint might be (width == height * 0.5)

      double priority = REQUIRED_PRIORITY;
      bool isConstantEditable = false;     ///< If true, the 'constant' aspect of the constraint will be an edit_variable.   TODO-EF: Actually implement this
      bool isMultiplierEditable = false;   ///< If true, the 'multiplier' aspect of the constraint will be an edit_variable. TODO-EF: Actually implement this
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
      
      // For Text and Images, we might want to constrain the size of the element to the size
      //    of its rendered text, or the size of the rendered image.
      ContentWidth, ContentHeight, ContentAspectRatio,

      NoTarget
   };
   
   static std::string StringFromConstraintTarget(Target target);
   static Target ConstraintTargetFromString(std::string name);

   UIConstraint();
   UIConstraint(UIConstrainable* primaryElement, UIConstrainable* secondaryElement, Target primaryTarget, Target secondaryTarget, const Options& options = Options{});

   UIConstrainable* GetPrimaryElement() const;
   Target           GetPrimaryTarget() const;
   
   UIConstrainable* GetSecondaryElement() const;
   Target           GetSecondaryTarget() const;
   
   // Produce a human-readable string that represents this constraint.
   std::string ToString() const;
   const Options& GetOptions() const;

private:
   Options mOptions;

   UIConstrainable* mPrimaryElement;    ///< First element being constrained.
   UIConstrainable* mSecondaryElement;  ///< Second element being constrained. NOTE: CAN BE THE SAME AS THE PRIMARY ELEMENT, OR NULL.

   Target mPrimaryTarget;         ///< Describes which aspect of each element is being constrained.
   Target mSecondaryTarget;
};

}; // namespace Engine

}; // namespace CubeWorld

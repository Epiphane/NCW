//
// BaseConstraint.h
//
// BaseConstraint – a wrapper around rhea::constraint with additional fields like
//                   ID. Lets us manage constraints in a map in the UIRoot.
//
// By Elliot Fiske
//

#pragma once

#include <rhea/constraint.hpp>

namespace CubeWorld
{

namespace Engine
{

//
// Base-class constraint. Lets us make custom constraints if we want to constrain
//    something besides UI.
//
class BaseConstraint {
public:
   //
   // Specifies the operator between the elements of the constraint
   //
   enum Relationship {
      Equal = 0,
      GreaterThanOrEqual,
      LessThanOrEqual
   };
   
   static constexpr double REQUIRED_PRIORITY = std::numeric_limits<double>::max();  ///< Default priority for constraints.
   static constexpr double HIGH_PRIORITY     = 1000;
   static constexpr double MEDIUM_PRIORITY   = 750;
   static constexpr double LOW_PRIORITY      = 500;
   
   BaseConstraint(std::string name, double priority = 0);
   
   rhea::constraint GetInternalConstraint() const;
   void SetInternalConstraint(rhea::constraint newConstraint);
   
   double GetPriority() const;
   void SetPriority(double newPriority);
   
   std::string GetName() const;
   void SetName(std::string newName);

protected:
   rhea::constraint mInternalConstraint;  ///< The actual internal rhea constraint
   
private:
   std::string mName;                     ///< Unique identifier for this constraint. Should be human readable + helpful for debugging.
   double mPriority;                      ///< Our version of weight. Simplified down to one number.
};

}; // namespace Engine

}; // namespace CubeWorld

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
   static constexpr double REQUIRED_PRIORITY = std::numeric_limits<double>::max();  ///< Default priority for constraints.
   static constexpr double HIGH_PRIORITY     = 1000;
   static constexpr double MEDIUM_PRIORITY   = 750;
   static constexpr double LOW_PRIORITY      = 500;
   
   BaseConstraint(std::string newId, double priority = 0);
   
   rhea::constraint GetInternalConstraint();
   void SetInternalConstraint(rhea::constraint newConstraint);
   
   double GetPriority();
   void SetPriority(double newPriority);
   
   void SetClean();
   
private:
   std::string mName;                     ///< Unique identifier for this constraint. Should be human readable + helpful for debugging.
   rhea::constraint mInternalConstraint;  ///< The actual internal rhea constraint
   double mPriority;                      ///< Our version of weight. Simplified down to one number.
   
   bool mbDirty;                          ///< If true, the mpRoot will update its solver with this constraint's new data this frame.
};

}; // namespace Engine

}; // namespace CubeWorld

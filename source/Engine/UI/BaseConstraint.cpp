//
// BaseConstraint.cpp
//
// BaseConstraint – a wrapper around rhea::constraint with additional fields like
//                   ID. Lets us manage constraints in a map in the UIRoot. 
//
// By Elliot Fiske
//

#include "BaseConstraint.h"

namespace CubeWorld
{

namespace Engine
{
   
rhea::constraint BaseConstraint::GetInternalConstraint() {
   return mInternalConstraint; 
}
   
/**
 * Set the internal constraint directly. Shouldn't be called on UIConstraints.
 */
void BaseConstraint::SetInternalConstraint(rhea::constraint newConstraint) { 
   mInternalConstraint = newConstraint; 
   mbDirty = true;
}
   
/**
 * Set the new priority for the constraint.
 *
 * If there is a conflict between constraints, the solver will discard the LOWEST
 *  priority constraint until the system becomes solvable.
 *
 * Note that in OUR system, we simplify priorities down to a single double value.
 *
 * Below required(), you can use BaseConstraint::HighPriority/MediumPriority/LowPriority which are set to
 *    1000, 750 and 500 respectively.
 */
void BaseConstraint::SetPriority(double newPriority) {
   mInternalConstraint.change_strength(rhea::symbolic_weight(newPriority, 0, 0));
   mbDirty = true;
}
   
double BaseConstraint::GetPriority() {
   return mPriority;
}
   
void BaseConstraint::SetClean() {
   mbDirty = false;
}   

}; // namespace Engine

}; // namespace CubeWorld

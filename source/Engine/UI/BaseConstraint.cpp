//
// BaseConstraint.cpp
//
// BaseConstraint – a wrapper around rhea::constraint with additional fields like
//                   ID. Lets us manage constraints in a map in the UIRootDep.
//
// By Elliot Fiske
//

#include "BaseConstraint.h"

namespace CubeWorld
{

namespace Engine
{

BaseConstraint::BaseConstraint(std::string name, double priority)
   : mName(name)
   , mPriority(priority)
{
}

rhea::constraint BaseConstraint::GetInternalConstraint() const {
   return mInternalConstraint;
}

//
// Set the internal constraint directly. Shouldn't be called on UIConstraints.
//
void BaseConstraint::SetInternalConstraint(rhea::constraint newConstraint) {
   mInternalConstraint = newConstraint;
   SetPriority(mPriority);
}

//
// Set the new priority for the constraint.
//
// If there is a conflict between constraints, the solver will discard the LOWEST
//  priority constraint until the system becomes solvable.
//
// Note that in OUR system, we simplify priorities down to a single double value.
//
// Below required(), you can use BaseConstraint::HighPriority/MediumPriority/LowPriority which are set to
//    1000, 750 and 500 respectively.
//
void BaseConstraint::SetPriority(double newPriority) {
   mInternalConstraint.change_strength(rhea::symbolic_weight(newPriority, newPriority, newPriority));
}

double BaseConstraint::GetPriority() const {
   return mPriority;
}

std::string BaseConstraint::GetName() const {
   return mName;
}

void BaseConstraint::SetName(std::string newName) {
   mName = newName;
}

}; // namespace Engine

}; // namespace CubeWorld

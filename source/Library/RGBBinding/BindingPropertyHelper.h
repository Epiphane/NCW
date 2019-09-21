// By Thomas Steinke

#pragma once

#include "BindingProperty.h"

namespace CubeWorld
{

namespace BindingPropertyHelper
{

//
// Compute the difference from one BindingProperty to another.
//
// This will not provide hints for removing properties, simply
// pointing out whatever fields exist in {to} that don't exist
// in {from}.
//
// Notes:
// If an array is changed, then the difference is the entirely
// new array - no in-array diffing is performed.
//
BindingProperty Difference(const BindingProperty& from, const BindingProperty& to);

}; // namespace BindingPropertyHelper

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <string>

#include "InputEvent.h"
#include "Transformer.h"

namespace CubeWorld
{

//
// Doesn't actually implement anything, but allows for inheriting one interface
// instead of 3 Transformers
//
class MouseInputTransformer
   : public Engine::Transformer<MouseDownEvent>
   , public Engine::Transformer<MouseUpEvent>
   , public Engine::Transformer<MouseClickEvent>
{
public:
   //
   // Transformer overrides. Must be implemented.
   //
   const MouseDownEvent TransformEventDown(const MouseDownEvent& evt) const override = 0;
   const MouseUpEvent TransformEventDown(const MouseUpEvent& evt) const override = 0;
   const MouseClickEvent TransformEventDown(const MouseClickEvent& evt) const override = 0;
};

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <functional>
#include <vector>

#include <RGBDesignPatterns/Singleton.h>

namespace CubeWorld
{

namespace Engine
{

class BaseTransformer {};

//
// Transformer is a simple interface used by EventManager::SetParent(). It's used
// mostly for mouse input transforming, specifically by StateWindow and SubFrame.
//
// Transformer provides two methods: TransformEventUp and TransformEventDown.
// TransformEventUp is called when an EventManager is about to emit its message up
//   to its parent, returning the event that should be propagated upwards.
// TransformEventDown is called when an EventManager is about to emit an event
//   to a child. The returned event is sent down to the child.
//
template<typename Event>
class Transformer : public BaseTransformer {
public:
   // Only override for swallowing events.
   virtual bool ShouldPropagateDown(const Event&) const { return true; }

   virtual const Event TransformEventDown(const Event& original) const
   {
      return original;
   }

   // Only override for swallowing events.
   virtual bool ShouldPropagateUp(const Event&) const { return true; }

   virtual const Event TransformEventUp(const Event& original) const
   {
      return original;
   }
};

}; // namespace Engine

}; // namespace CubeWorld

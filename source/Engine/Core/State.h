// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include "Config.h"
#include "Input.h"
#include "../Entity/EntityManager.h"
#include "../Event/EventManager.h"
#include "../System/SystemManager.h"
#include "../Graphics/Camera.h"

namespace CubeWorld
{

namespace Engine
{

class GameObject;

class State {
public:
   State();
   ~State();

   bool initialized = false;
   virtual void Start() = 0;
   virtual void Update(TIMEDELTA dt);

   //
   // Emit an event to the state. Intentionally disallows
   // referencing mEvents directly, because there's too much
   // opportunity for misusing that dependency, i.e. subscribing
   // permanently to a temporary event manager.
   //
   template <typename E>
   inline void Emit(const E& evt)
   {
      mEvents.Emit<E>(evt);
   }

protected:
   EventManager mEvents;
   EntityManager mEntities;
   SystemManager mSystems;
};

}; // namespace Engine

}; // namespace CubeWorld

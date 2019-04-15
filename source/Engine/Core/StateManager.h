// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <RGBDesignPatterns/Singleton.h>
#include "Config.h"
#include "State.h"

namespace CubeWorld
{

namespace Engine
{

class StateManager : public Singleton<StateManager>
{
public:
   StateManager();
   ~StateManager();

   void Shutdown();

public:
   void SetState(State* state);
   void SetState(std::unique_ptr<State>&& state);

   virtual void Update(TIMEDELTA dt);

   //
   // Emit an event to the current state.
   //
   template <typename E>
   inline void Emit(const E& evt)
   {
      mState->Emit<E>(evt);
   }

   template <typename E, typename ... Args>
   void Emit(Args && ... args)
   {
      Emit(E(std::forward<Args>(args)...));
   }

private:
   std::unique_ptr<State> mOwned;
   std::unique_ptr<State> mOwnNext;

   State* mState;
   State* mNext;
};

}; // namespace Engine

}; // namespace CubeWorld

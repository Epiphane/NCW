// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include "Config.h"
#include "Singleton.h"
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

   void Update(TIMEDELTA dt);

private:
   std::unique_ptr<State> mOwned;
   std::unique_ptr<State> mOwnNext;

   State* mState;
   State* mNext;
};

}; // namespace Engine

}; // namespace CubeWorld

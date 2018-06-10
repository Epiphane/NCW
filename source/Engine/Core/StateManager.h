// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include "Config.h"
#include "Input.h"
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

   void Update(TIMEDELTA dt);

private:
   State* mState;
   State* mNext;
};

}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Input.h>

#include "Singleton.h"
#include "State.h"
#include <Engine/GameObject/GameObject.h>

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

   void Update(const Input::InputManager* input, double dt);
   void Render(double dt);

private:
   State* mState;
   State* mNext;
};

}; // namespace Engine

}; // namespace CubeWorld

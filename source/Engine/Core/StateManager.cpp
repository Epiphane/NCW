// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <Engine/Logger/Logger.h>

#include "StateManager.h"

namespace CubeWorld
{

namespace Engine
{

StateManager::StateManager() : mState(nullptr), mNext(nullptr)
{
}

StateManager::~StateManager()
{
   // Ensure we have been shutdown previously
   assert(!mState);
}

void StateManager::Shutdown()
{
   mState = nullptr;
}

void StateManager::SetState(State* state)
{
   mNext = state;
}

void StateManager::Update(const Input::InputManager* input, double dt)
{
   if (mNext != nullptr)
   {
      mState = mNext;
      mNext = nullptr;
      mState->Start();
   }

   if (mState == nullptr)
   {
      return;
   }

   mState->Update(input, dt);
}

void StateManager::Render(double dt)
{
   if (mState == nullptr)
   {
      return;
   }

   mState->Render(dt);
}

}; // namespace Engine

}; // namespace CubeWorld
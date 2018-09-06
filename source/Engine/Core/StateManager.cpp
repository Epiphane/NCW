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

void StateManager::SetState(std::unique_ptr<State>&& state)
{
   mOwnNext = std::move(state);
   SetState(mOwnNext.get());
}

void StateManager::Update(TIMEDELTA dt)
{
   if (mNext != nullptr)
   {
      mState = mNext;
      mOwned = std::move(mOwnNext);
      mNext = nullptr;
      mOwnNext.reset();
      mState->Start();
   }

   if (mState == nullptr)
   {
      return;
   }

   mState->Update(dt);
}

}; // namespace Engine

}; // namespace CubeWorld
// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include "../Core/Config.h"

#include "State.h"

namespace CubeWorld
{

namespace Engine
{

State::State() : mEntities(mEvents), mSystems(mEntities, mEvents)
{
}

State::~State()
{
}

void State::Update(TIMEDELTA dt)
{
   mSystems.UpdateAll(dt);
}

}; // namespace Engine

}; // namespace CubeWorld
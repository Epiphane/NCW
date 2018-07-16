// By Thomas Steinke

#include "SystemManager.h"

namespace CubeWorld
{

namespace Engine
{

void SystemManager::UpdateAll(TIMEDELTA dt)
{
   assert(mInitialized);
   for (auto& pair : mSystems)
   {
      pair.second->Update(mEntityManager, mEventManager, dt);
   }
}

void SystemManager::Configure()
{
   assert(!mInitialized);
   for (auto& pair : mSystems)
   {
      pair.second->Configure(mEntityManager, mEventManager);
   }
   mInitialized = true;
}

}; // namespace Engine

}; // namespace CubeWorld

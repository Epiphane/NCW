// By Thomas Steinke

#pragma once

#include <assert.h>
#include <memory>
#include <unordered_map>

#include "../Entity/EntityManager.h"
#include "../Event/EventManager.h"
#include "System.h"

namespace CubeWorld
{

namespace Engine
{

class SystemManager
{
public:
   SystemManager(EntityManager& entityManager, EventManager& eventManager)
      : mInitialized(false)
      , mEntityManager(entityManager)
      , mEventManager(eventManager)
   {};

   // Add a system to the manager. It will live for as long as the system manager does.
   template<typename S, typename ...Args>
   void Add(Args&& ... args)
   {
      std::unique_ptr<S> system(new S(std::forward<Args>(args) ...));
      mSystems.insert(std::make_pair(S::GetFamily(), std::move(system)));
   }

   // Retrieve a System, if it exists.
   template<typename S>
   S* Get()
   {
      auto it = mSystems.find(S::GetFamily());
      assert(it != mSystems.end());
      return static_cast<S*>(it->second.get());
   }

   // Update all systems.
   void UpdateAll(TIMEDELTA dt);

   // Configure system. Call once after adding all systems.
   void Configure();

private:
   bool mInitialized;
   EntityManager& mEntityManager;
   EventManager& mEventManager;
   std::unordered_map<BaseSystem::Family, std::shared_ptr<BaseSystem>> mSystems;
};

}; // namespace Engine

}; // namespace CubeWorld

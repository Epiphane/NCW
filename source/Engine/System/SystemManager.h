// By Thomas Steinke

#pragma once

#include <assert.h>
#include <memory>
#include <unordered_map>

#include "../Core/Timer.h"
#include "../Entity/EntityManager.h"
#include "../Event/EventManager.h"
#include "System.h"

#ifndef CUBEWORLD_BENCHMARK_SYSTEMS
#define CUBEWORLD_BENCHMARK_SYSTEMS 1
#endif

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
   S* Add(Args&& ... args)
   {
      std::unique_ptr<S> system(new S(std::forward<Args>(args) ...));

      mSystems.push_back(std::move(system));
#if CUBEWORLD_BENCHMARK_SYSTEMS
      std::string name = typeid(S).name();

      // Cut off "class Cubeworld::"
      name = name.substr(17);

      mBenchmarks.push_back(std::make_pair(std::string(name), Timer<100>()));
#endif
      return (S*)mSystems.back().get();
   }

   // Update all systems.
   void UpdateAll(TIMEDELTA dt);

#if CUBEWORLD_BENCHMARK_SYSTEMS
   std::vector<std::pair<std::string, double>> GetBenchmarks();
#endif

   // Configure system. Call once after adding all systems.
   void Configure();

private:
   bool mInitialized;
   EntityManager& mEntityManager;
   EventManager& mEventManager;

   std::vector<std::unique_ptr<BaseSystem>> mSystems;
#if CUBEWORLD_BENCHMARK_SYSTEMS
   std::vector<std::pair<std::string, Timer<100>>> mBenchmarks;
#endif
};

}; // namespace Engine

}; // namespace CubeWorld

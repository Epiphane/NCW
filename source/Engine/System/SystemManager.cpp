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
#if CUBEWORLD_BENCHMARK_SYSTEMS
      std::pair<std::string, Timer<100>>& benchmark = mBenchmarks.at(pair.first);
      benchmark.second.Reset();
#endif
      pair.second->Update(mEntityManager, mEventManager, dt);
#if CUBEWORLD_BENCHMARK_SYSTEMS
      benchmark.second.Elapsed();
#endif
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

#if CUBEWORLD_BENCHMARK_SYSTEMS
std::vector<std::pair<std::string, double>> SystemManager::GetBenchmarks()
{
   std::vector<std::pair<std::string, double>> benchmarks;
   for (auto& pair : mBenchmarks)
   {
      benchmarks.push_back(std::make_pair(pair.second.first, pair.second.second.Average()));
   }
   return benchmarks;
}
#endif

}; // namespace Engine

}; // namespace CubeWorld

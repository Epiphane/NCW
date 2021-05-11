// By Thomas Steinke

#include "SystemManager.h"

namespace CubeWorld
{

namespace Engine
{

void SystemManager::UpdateAll(TIMEDELTA dt)
{
#if CUBEWORLD_BENCHMARK_SYSTEMS
    glFinish();
#endif
    assert(mInitialized);
    for (size_t i = 0; i < mSystems.size(); ++i)
    {
#if CUBEWORLD_BENCHMARK_SYSTEMS
        std::pair<std::string, Timer<100>>& benchmark = mBenchmarks[i];
        benchmark.second.Reset();
#endif
        mSystems[i]->Update(mEntityManager, mEventManager, dt);
#if CUBEWORLD_BENCHMARK_SYSTEMS
        glFinish();
        benchmark.second.Elapsed();
#endif
    }
}

void SystemManager::Configure()
{
   assert(!mInitialized);
   for (auto& system : mSystems)
   {
      system->Configure(mEntityManager, mEventManager);
   }
   mInitialized = true;
}

#if CUBEWORLD_BENCHMARK_SYSTEMS
std::vector<std::pair<std::string, double>> SystemManager::GetBenchmarks()
{
   std::vector<std::pair<std::string, double>> benchmarks;
   for (auto& benchmark : mBenchmarks)
   {
      benchmarks.push_back(std::make_pair(benchmark.first, benchmark.second.Average()));
   }
   return benchmarks;
}
#endif

}; // namespace Engine

}; // namespace CubeWorld

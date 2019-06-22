// By Thomas Steinke

#pragma once

#include <Engine/System/System.h>
#include "../DebugHelper.h"
#include "../Components/AnimationController.h"

namespace CubeWorld
{

class AnimationSystem : public Engine::System<AnimationSystem> {
public:
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events);
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt);

private:
   std::unique_ptr<DebugHelper::MetricLink> mMetric;
   std::unique_ptr<DebugHelper::MetricLink> mMetric2;
   std::unique_ptr<DebugHelper::MetricLink> mMetric3;
   std::unique_ptr<DebugHelper::MetricLink> mMetric4;
   std::string mCurrent;
   std::string mNext;
   double mTime;
   double mTransitionCurrent;
};

}; // namespace CubeWorld

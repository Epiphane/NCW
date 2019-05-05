// By Thomas Steinke

#pragma once

#include <unordered_map>

#include <Engine/Entity/Component.h>
#include <Engine/System/System.h>
#include <Shared/Components/AnimationController.h>
#include <Shared/Components/Skeleton.h>
#include <Shared/Components/SkeletonAnimations.h>

#include "AnimationSystem.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

//
// SimpleAnimationController doesn't do any fancy stuff with Skeletons or SkeletonAnimations.
//
struct SimpleAnimationController : public AnimationControllerBase, public Engine::Component<SimpleAnimationController> {
public:
   // Types
   using Bone = Skeleton::Bone;

   struct State {
      std::string entity;
      std::string name;
      std::string next;
      std::string stance;

      double length;
      std::vector<SkeletonAnimations::Keyframe> keyframes;
   };

   struct Stance {
      std::string name;
      std::string parent;
      std::vector<Bone> bones;
   };

public:
   void Reset();
   void UpdateSkeletonStates();

   void Play(const std::string& state, double startTime = 0.0);
   void TransitionTo(const std::string& state, double transitionTime = 0.0, double startTime = 0.0);

   // Skeleton and model objects
   void AddSkeleton(Engine::ComponentHandle<Skeleton> skeleton);
   void AddAnimations(Engine::ComponentHandle<SkeletonAnimations> animations);

private:
   friend class Dock;
   friend class Sidebar;
   friend class SimpleAnimationSystem;
   std::vector<Engine::ComponentHandle<Skeleton>> skeletons;
   std::vector<Engine::ComponentHandle<SkeletonAnimations>> animations;

   std::unordered_map<std::string, Stance> stances;
   std::unordered_map<std::string, State> states;

public:
   // Animation State
   std::string current;
   double time;

   // If transition > 0, this defines the state we're transitioning to,
   // as well as the amount of time remaining in that transition.
   std::string next;
   double transitionCurrent;
   double transitionStart;
   double transitionEnd;
};

class SimpleAnimationSystem : public Engine::System<SimpleAnimationSystem> {
public:
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

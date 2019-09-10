// By Thomas Steinke

#pragma once

#include <unordered_map>

#include <Engine/Entity/Component.h>
#include <Engine/Graphics/ParticleSystem.h>
#include <Engine/System/System.h>
#include <Shared/Components/AnimationController.h>
#include <Shared/Components/Skeleton.h>
#include <Shared/Components/SkeletonAnimations.h>
#include <Shared/Systems/SimpleParticleSystem.h>

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

struct AnimationSystemController : public Engine::Component<AnimationSystemController>
{
   //
   // Editing switches. Pause all animations (but do not disable) and disable transitions.
   //
   double speed = 1.0;
   bool paused = true;
   bool seamlessLoop = true;
   TIMEDELTA nextTick = 0;
};

//
// SimpleAnimationController doesn't do any fancy stuff with Skeletons or SkeletonAnimations.
//
struct SimpleAnimationController : public AnimationControllerBase, public Engine::Component<SimpleAnimationController> {
public:
   // Types
   using Bone = Skeleton::Bone;

   //
   // References an emitter in the MultipleParticleEmitters component.
   //
   struct EmitterRef {
      std::string bone;
      size_t emitter;
      double start;
      double end;
   };

   struct State {
      std::string entity;
      std::string name;
      std::string next;
      std::string stance;

      double length = 0.0;
      std::vector<SkeletonAnimations::Keyframe> keyframes;
      std::vector<EmitterRef> emitters;
   };

   struct Stance {
      std::string name;
      std::string parent;
      std::vector<Bone> bones;
   };

public:
   SimpleAnimationController();
   SimpleAnimationController(Engine::ComponentHandle<MultipleParticleEmitters> emitters);
   void Reset();
   void UpdateSkeletonStates();

   void Play(const std::string& state, double startTime = 0.0) override;
   void TransitionTo(const std::string& state, double transitionTime = 0.0, double startTime = 0.0) override;

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
   double cooldown;

   // If transition > 0, this defines the state we're transitioning to,
   // as well as the amount of time remaining in that transition.
   std::string next;
   double transitionCurrent;
   double transitionStart;
   double transitionEnd;

   Engine::ComponentHandle<MultipleParticleEmitters> emitters;
};

class SimpleAnimationSystem : public Engine::System<SimpleAnimationSystem> {
public:
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

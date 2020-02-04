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
   struct Emitter : public MultipleParticleEmitters::Emitter {
      using MultipleParticleEmitters::Emitter::Emitter;

      std::string entity;
      std::string bone;
      double start;
      double end;
   };

   struct Event : public SkeletonAnimations::Event
   {
      Event(const SkeletonAnimations::Event& evt) : SkeletonAnimations::Event(evt) {};

      std::string entity;
   };

   struct Transition : public SkeletonAnimations::Transition
   {
      Transition() : SkeletonAnimations::Transition() {};
      Transition(const SkeletonAnimations::Transition& tra) : SkeletonAnimations::Transition(tra) {};

      std::string entity;
   };

   struct State {
      std::string entity;
      std::string name;
      std::string next;
      std::string stance;
      bool isDefault = false;
      bool loop = true;
      double length = 0.0;

      std::vector<SkeletonAnimations::Keyframe> keyframes;
      std::vector<Emitter> particles;
      std::vector<Event> events;
      std::vector<Transition> transitions;
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
   friend class AnimationDebugSystem;
   std::vector<Engine::ComponentHandle<Skeleton>> skeletons;
   std::vector<Engine::ComponentHandle<SkeletonAnimations>> animations;

   std::unordered_map<std::string, Stance> stances;
   std::vector<State> states;

public:
   // Animation State
   size_t current = 0;
   double time;
   double cooldown;

   // If transition > 0, this defines the state we're transitioning to,
   // as well as the amount of time remaining in that transition.
   size_t next = 0;
   double transitionCurrent;
   double transitionStart;
   double transitionEnd;

   Engine::ComponentHandle<MultipleParticleEmitters> emitterContainer;
};

class SimpleAnimationSystem : public Engine::System<SimpleAnimationSystem> {
public:
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

namespace meta
{

using CubeWorld::Editor::AnimationStation::SimpleAnimationController;

template <>
inline auto registerMembers<SimpleAnimationController::State>()
{
   return members(
      member("name", &SimpleAnimationController::State::name),
      member("next", &SimpleAnimationController::State::next),
      member("length", &SimpleAnimationController::State::length),
      member("default", &SimpleAnimationController::State::isDefault),
      member("loop", &SimpleAnimationController::State::loop)
   );
}

template <>
inline auto registerMembers<SimpleAnimationController::Transition>()
{
   return registerMembers<SkeletonAnimations::Transition>();
}

template <>
inline auto registerMembers<SimpleAnimationController::Emitter>()
{
   return std::tuple_cat(
      registerMembers<CubeWorld::Engine::ParticleSystem>(),
      members(
         member("start", &SimpleAnimationController::Emitter::start),
         member("end", &SimpleAnimationController::Emitter::end),
         member("bone", &SimpleAnimationController::Emitter::bone)
      )
   );
}

template <>
inline auto registerMembers<SimpleAnimationController::Event>()
{
   return registerMembers<SkeletonAnimations::Event>();
}

}; // namespace meta

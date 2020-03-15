// By Thomas Steinke

#pragma once

#include <unordered_map>
#include <Shared/Systems/SimpleParticleSystem.h>

#include "SkeletonAnimations.h"
#include "Skeleton.h"

namespace CubeWorld
{

//
// AnimationControllerBase has the common parts in an AnimationController, like
// transition parameters.
//
struct AnimationControllerBase {
   virtual ~AnimationControllerBase() = default;

   // Interface
   virtual void Play(const std::string& state, double startTime = 0.0) = 0;
   virtual void TransitionTo(const std::string& state, double transitionTime = 0.0, double startTime = 0.0) = 0;

   // Common implementation
   float GetFloatParameter(const std::string& name) { return floatParams[name]; }
   void SetParameter(const std::string& name, float val) { floatParams[name] = val; }
   
   bool GetBoolParameter(const std::string& name) { return boolParams[name]; }
   void SetBoolParameter(const std::string& name, bool val) { boolParams[name] = val; }

   // Animation FSM parameters
   std::unordered_map<std::string, float> floatParams;
   std::unordered_map<std::string, bool> boolParams;
};

//
// AnimationController combined the data from Skeleton and SkeletonAnimations components into
// a more efficient structure. For this reason it's also a lot harder to modify during runtime.
//
// If you desire that functionality, the SimpleAnimationController and associated SimpleAnimationSystem
// in AnimationStation is much more useful.
//
struct AnimationController : public AnimationControllerBase, public Engine::Component<AnimationController> {
public:
   struct Keyframe {
      double time;
      std::optional<double> maxSpeed;
      std::vector<glm::vec3> positions;
      std::vector<glm::vec3> rotations;
      std::vector<glm::vec3> scales;
   };

   //
   // References an emitter in the MultipleParticleEmitters component.
   //
   struct ParticleEffect : public SkeletonAnimations::ParticleEffect {
      Engine::ComponentHandle<Engine::Transform> spawned;
   };

   struct State {
      std::string name;
      std::string next;
      size_t stance;
      bool loop;
      bool maskTorso;

      double length;
      std::vector<Keyframe> keyframes;
      std::vector<SkeletonAnimations::Transition> transitions;
      std::vector<SkeletonAnimations::Event> events;
      std::vector<ParticleEffect> effects;
   };

   struct Stance {
      std::string name;
      std::string parent;
      std::vector<size_t> parents;
      std::vector<glm::vec3> positions;
      std::vector<glm::vec3> rotations;
      std::vector<glm::vec3> scales;
   };

public:
   // For initializing and loading data
   AnimationController();

   void Reset();

public:
   // Info and manipulation
   void AddSkeleton(Engine::ComponentHandle<Skeleton> skeleton);
   void AddAnimations(Engine::ComponentHandle<SkeletonAnimations> animations);

public:
   void Play(const std::string& state, double startTime = 0.0) override;
   void TransitionTo(const std::string& state, double transitionTime = 0.0, double startTime = 0.0) override;

private:
   // Skeleton and model objects
   friend class AnimationSystem;
   friend class AnimationApplicator;
   friend class AnimationEventSystem;
   friend class AnimationEventDebugSystem;
   friend class WalkAnimationSystem;
   std::vector<Engine::ComponentHandle<Skeleton>> skeletons;

   // Pair of skeleton ID and bone ID
   std::vector<size_t> skeletonRootId;

public:
   // Stuff for WalkAnimationSystem
   double walkAnimationProgress;

public:
   std::vector<std::string> bones;
   std::unordered_map<std::string, size_t> boneLookup;

public:
   // Combined skeleton data.
   std::vector<State> states;
   std::unordered_map<std::string, size_t> stateLookup;

   std::vector<Stance> stances;

public:
   // Animation State
   size_t current;
   size_t prev;
   double time;

   glm::vec3 lastBasePosition;
   glm::vec3 lastTransitionPosition;

   // If transition > 0, this defines the state we're transitioning to,
   // as well as the amount of time remaining in that transition.
   size_t next;
   double transitionCurrent;
   double transitionStart, transitionEnd;
};

}; // namespace CubeWorld

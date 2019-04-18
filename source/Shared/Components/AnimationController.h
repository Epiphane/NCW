// By Thomas Steinke

#pragma once

#include <unordered_map>

#include <Engine/Entity/Component.h>
#include "DeprecatedSkeleton.h"
#include "VoxModel.h"

namespace CubeWorld
{

//
// AnimationController contains potentially multiple AnimatedSkeletons and is attached to an entity.
//
struct AnimationController : public Engine::Component<AnimationController> {
public:
   using BoneID = size_t;

public:
   struct Keyframe {
      double time;
      std::vector<glm::vec3> positions;
      std::vector<glm::vec3> rotations;
      std::vector<glm::vec3> scales;
   };

   using Transition = DeprecatedSkeleton::Transition;

   struct State {
      std::string name;
      std::string next;
      size_t stance;
      size_t skeletonId;

      double length;
      std::vector<Keyframe> keyframes;
      std::vector<Transition> transitions;
   };

   struct Stance {
      std::string name;
      std::string inherit;
      std::vector<size_t> parents;
   };

public:
   // For initializing and loading data
   AnimationController();

   void Reset();

   // Ensure that skeletons reflect the state of the animation controller.
   void UpdateSkeletonStates();

public:
   // Info and manipulation
   State& GetCurrentState();

   void AddSkeleton(Engine::ComponentHandle<DeprecatedSkeleton> skeleton);
   size_t NumSkeletons() { return skeletons.size(); }

   void AddState(Engine::ComponentHandle<DeprecatedSkeleton> skeleton, const DeprecatedSkeleton::State& state);

public:
   Stance& GetStance(const std::string& name);

public:
   // Bone and skeleton lookup
   Engine::ComponentHandle<DeprecatedSkeleton> GetSkeleton(size_t ndx) { return skeletons[ndx]; }
   Engine::ComponentHandle<DeprecatedSkeleton> GetSkeletonForBone(BoneID id);
   DeprecatedSkeleton::Bone* GetBone(BoneID id);
   BoneID NextBone(BoneID id);
   BoneID PrevBone(BoneID id);
   BoneID ParentBone(BoneID id);

public:
   float GetFloatParameter(const std::string& name) { return floatParams[name]; }
   void SetParameter(const std::string& name, float val) { floatParams[name] = val; }
   
   bool GetBoolParameter(const std::string& name) { return boolParams[name]; }
   void SetBoolParameter(const std::string& name, bool val) { boolParams[name] = val; }

public:
   void Play(const std::string& state, double startTime = 0.0);
   void TransitionTo(const std::string& state, double transitionTime = 0.0, double startTime = 0.0);

private:
   // Skeleton and model objects
   friend class BaseAnimationSystem;
   std::vector<Engine::ComponentHandle<DeprecatedSkeleton>> skeletons;
   // Pair of skeleton ID and bone ID
   std::vector<size_t> skeletonRootId;

public:
   std::vector<std::string> bones;
   std::unordered_map<std::string, size_t> bonesByName;

public:
   // Combined skeleton data.
   std::vector<State> states;
   std::unordered_map<std::string, size_t> statesByName;

   std::vector<Stance> stances;

   // Animation FSM parameters
   std::unordered_map<std::string, float> floatParams;
   std::unordered_map<std::string, bool> boolParams;

public:
   // Animation State
   size_t current;
   double time;

   // If transition > 0, this defines the state we're transitioning to,
   // as well as the amount of time remaining in that transition.
   size_t next;
   double transitionCurrent;
   double transitionStart, transitionEnd;
};

}; // namespace CubeWorld

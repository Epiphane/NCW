// By Thomas Steinke

#pragma once

#include <unordered_map>

#include <Engine/Entity/Component.h>
#include "AnimatedSkeleton.h"
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

   struct Transition {
      struct Trigger {
         typedef enum {
            FloatGte, FloatLt, Bool
         } Type;

         Type type;
         std::string parameter;
         union {
            float floatVal;
            bool boolVal;
         };
      };

      std::string destination;
      double time;
      std::vector<Trigger> triggers;
   };

   struct State {
      std::string name;

      double length;
      std::vector<Keyframe> keyframes;
      std::vector<Transition> transitions;
   };

public:
   // For initializing and loading data
   AnimationController();

   void Reset();

public:
   // Info and manipulation
   AnimationController::State& GetCurrentState();

   void AddSkeleton(Engine::ComponentHandle<AnimatedSkeleton> skeleton);
   size_t NumSkeletons() { return skeletons.size(); }
   Engine::ComponentHandle<AnimatedSkeleton> GetSkeleton(size_t ndx) { return skeletons[ndx]; }
   Engine::ComponentHandle<AnimatedSkeleton> GetSkeletonForBone(BoneID id);

   AnimatedSkeleton::Bone* GetBone(BoneID id);
   BoneID NextBone(BoneID id);
   BoneID PrevBone(BoneID id);
   BoneID ParentBone(BoneID id);

public:
   void SetParameter(const std::string& name, float val) { floatParams[name] = val; }
   void SetParameter(const std::string& name, bool val) { boolParams[name] = val; }

public:
   void Play(const std::string& state, double startTime = 0.0);
   void TransitionTo(const std::string& state, double transitionTime = 0.0, double startTime = 0.0);

private:
   // Skeleton and model objects
   friend class BaseAnimationSystem;
   std::vector<Engine::ComponentHandle<AnimatedSkeleton>> skeletons;
   std::unordered_map<std::string, size_t> bonesByName;
   // Pair of skeleton ID and bone ID
   std::vector<size_t> skeletonRootId;
   std::vector<std::pair<size_t, size_t>> skeletonParents;
   size_t numBones;

   std::unordered_map<std::string, size_t> statesByName;

public:
   // Combined skeleton data.
   std::vector<State> states;

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

// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <unordered_map>
#include <vector>

#include <Engine/Core/Input.h>
#include <Engine/System/System.h>
#include "../Components/CubeModel.h"

namespace CubeWorld
{

namespace Game
{

   struct AnimatedSkeleton : public Engine::Component<AnimatedSkeleton> {
      struct Keyframe {
         double time;
         std::vector<glm::mat4> matrixes;
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

      struct Bone {
         std::string name;

         glm::vec3 position;
         glm::vec3 rotation;
         glm::mat4 matrix;

         // Reference parent and each child by ID.
         size_t parent;
         std::vector<size_t> children;
      };

   public:
      void ComputeBoneMatrix(size_t boneId);
      
      AnimatedSkeleton(const std::string& file);
      void Load(const std::string& file);
      
      std::vector<State> states;
      std::unordered_map<std::string, size_t> statesByName;

      // Index 0 is the root of the skeleton.
      std::vector<Bone> bones;
      std::unordered_map<std::string, size_t> bonesByName;

      // Animation FSM parameters
      std::unordered_map<std::string, float> floatParams;
      std::unordered_map<std::string, bool> boolParams;

   public:
      void SetParameter(const std::string& name, float val) { floatParams[name] = val; }

   public:
      size_t current;
      double time;

      // If transition > 0, this defines the state we're transitioning to,
      // as well as the amount of time remaining in that transition.
      size_t next;
      double transitionCurrent;
      double transitionStart, transitionEnd;

      std::string CurrentState() { return states[current].name; }

   public:
      // CubeModels attached to the skeleton.
      // For now, a model can only be attached to one bone, but for extensibility
      // the interface pretends to allow multiple.
      using BoneWeights = std::vector<std::pair<std::string, float>>;
      void AddModel(const BoneWeights& bones, const std::string& model);
      void AddModel(const BoneWeights& bones, const std::string& model, glm::vec3 tint);

      struct ModelAttachment {
         size_t bone;
         float weight;
         CubeModelInfo* model;
         glm::vec3 tint;
      };
      std::vector<ModelAttachment> models;

   public:
      void Play(const std::string& state, double startTime = 0.0);
      void TransitionTo(const std::string& state, double transitionTime = 0.0, double startTime = 0.0);
   };

   struct BoneAttachment : public Engine::Component<BoneAttachment> {
      Engine::ComponentHandle<AnimatedSkeleton> skeleton;
      std::string bone;
      glm::vec3 relativePosition;
      glm::vec3 relativeRotation;

      BoneAttachment(
         const Engine::ComponentHandle<AnimatedSkeleton>& skeleton,
         const std::string& bone,
         const glm::vec3& relativePosition = glm::vec3(0),
         const glm::vec3& relativeRotation = glm::vec3(0)
      )
         : skeleton(skeleton)
         , bone(bone)
         , relativePosition(relativePosition)
         , relativeRotation(relativeRotation)
      {};
   };
   
   class AnimationSystem : public Engine::System<AnimationSystem> {
   public:
      AnimationSystem(Engine::Input::InputManager* input) : mInput(input) {}
      ~AnimationSystem() {}
      
      void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
      
   private:
      Engine::Input::InputManager* mInput;
   };

}; // namespace Game

}; // namespace CubeWorld

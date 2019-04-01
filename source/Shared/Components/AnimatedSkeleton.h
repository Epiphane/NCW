// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <unordered_map>
#include <vector>

#include <Engine/Entity/ComponentHandle.h>
#include <Engine/Entity/EntityManager.h>
#include "../Components/VoxModel.h"
#include "../Helpers/json.hpp"
#include "../Voxel.h"

namespace CubeWorld
{

struct AnimatedSkeleton : Engine::Component<AnimatedSkeleton>  {
public:
   struct Keyframe {
      double time;
      std::unordered_map<std::string, glm::vec3> positions;
      std::unordered_map<std::string, glm::vec3> rotations;
      std::unordered_map<std::string, glm::vec3> scales;
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

      // Original position and rotation are stored for the editor.
      // Memory _really_ shouldn't matter that much since its 6 floats
      // per bone, but this can potentially be revisited
      glm::vec3 originalPosition;
      glm::vec3 originalRotation;
      glm::vec3 originalScale;

      glm::vec3 position;
      glm::vec3 rotation;
      glm::vec3 scale;
      glm::mat4 matrix;

      // Reference parent and each child by ID.
      size_t parent;
      std::vector<size_t> children;
   };

public:
   void ComputeBoneMatrix(size_t boneId);
   
   AnimatedSkeleton();
   AnimatedSkeleton(const std::string& file) { Load(file); }
   AnimatedSkeleton(Engine::ComponentHandle<VoxModel> model) : model(model) {}
   AnimatedSkeleton(const std::string& file, Engine::ComponentHandle<VoxModel> model) : model(model) { Load(file); }
   void Reset();
   void Load(const std::string& file);
   //void Load(const std::string& workingDirectory, const nlohmann::json& data);
   std::string Serialize();

public:
   // Data
   std::string name;
   std::string modelFilename;
   std::string parentFilename;
   std::string parentBone;

   std::vector<State> states;
   std::unordered_map<std::string, size_t> statesByName;

   // Index 0 is the root of the skeleton.
   std::vector<Bone> bones;
   std::unordered_map<std::string, size_t> bonesByName;

   // Attach a VoxModel component directly. Note that it doesn't _have_
   // to be hosted by the same entity as this AnimatedSkeleton.
   Engine::ComponentHandle<VoxModel> model;
};

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <unordered_map>
#include <vector>

#include <Engine/Entity/ComponentHandle.h>
#include <Engine/Entity/EntityManager.h>
#include "../Components/VoxModel.h"
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
            double doubleVal;
            bool boolVal;
         };
      };

      std::string destination;
      double time;
      std::vector<Trigger> triggers;
   };

   struct State {
      std::string name;
      std::string next;
      std::string stance;

      double length;
      std::vector<Keyframe> keyframes;
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

   struct Stance {
      std::string name;
      std::string inherit;
      std::string parentBone; // If this is a child skeleton

      std::vector<Bone> bones;
   };

public:
   void ComputeBoneMatrix(size_t boneId);
   
   AnimatedSkeleton();
   AnimatedSkeleton(const std::string& file) { Load(file); }
   AnimatedSkeleton(Engine::ComponentHandle<VoxModel> model) : model(model) {}
   AnimatedSkeleton(const std::string& file, Engine::ComponentHandle<VoxModel> model) : model(model) { Load(file); }
   void Reset();
   void Load(const std::string& file);
   std::string Serialize();

public:
   // Data
   std::string name;
   std::string modelFilename;
   std::string parent;
   std::string parentBone;

   std::vector<State> states;
   std::unordered_map<std::string, size_t> statesByName;
   std::unordered_map<std::string, std::vector<Transition>> transitions;

   // Index 0 is the root of the skeleton.
   // bones describe the stance-less state of this skeleton.
   std::vector<Bone> bones;
   std::unordered_map<std::string, size_t> bonesByName;

   std::vector<Stance> stances;
   std::unordered_map<std::string, size_t> stancesByName;

   // Attach a VoxModel component directly. Note that it doesn't _have_
   // to be hosted by the same entity as this AnimatedSkeleton.
   Engine::ComponentHandle<VoxModel> model;
};

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <unordered_map>
#include <vector>

#include <RGBBinding/BindingProperty.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Engine/Entity/EntityManager.h>
#include "../Components/VoxModel.h"
#include "../Voxel.h"

namespace CubeWorld
{

struct Skeleton : Engine::Component<Skeleton>  {
   // Types
   /*
   struct Keyframe {
      double time;
      std::unordered_map<std::string, glm::vec3> positions;
      std::unordered_map<std::string, glm::vec3> rotations;
      std::unordered_map<std::string, glm::vec3> scales;
   };

   struct State {
      std::string name;
      std::string next;
      std::string stance;

      double length;
      std::vector<Keyframe> keyframes;
   };

   struct Transition {
      struct Trigger {
         enum {
            GreaterThan,
            LessThan,
            Bool
         } type;
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
   */

   struct Bone {
      std::string name;
      std::string parent;
      glm::vec3 position;
      glm::vec3 rotation;
      glm::vec3 scale;
      glm::mat4 matrix;
   };

   struct Stance {
      std::string name;
      std::string parent;
      std::unordered_map<std::string, std::string> parents;
      std::unordered_map<std::string, glm::vec3> positions;
      std::unordered_map<std::string, glm::vec3> rotations;
      std::unordered_map<std::string, glm::vec3> scales;
   };

public:
   static void Transform(glm::mat4& matrix, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

public:
   Skeleton();
   Skeleton(Engine::ComponentHandle<VoxModel> model);
   Skeleton(const BindingProperty& data);
   Skeleton(const BindingProperty& data, Engine::ComponentHandle<VoxModel> model);

   void Reset();
   void Load(const BindingProperty& data);

   BindingProperty Serialize();

public:
   // Data
   std::string name;
   std::string defaultModel;
   std::string parent;

   // The current state of the skeleton
   std::vector<Bone> bones;

   // The original state of the skeleton
   std::vector<Bone> original;
   std::unordered_map<std::string, size_t> boneLookup;

   std::unordered_map<std::string, Stance> stances;

   //std::unordered_map<std::string, State> states;
   //std::unordered_map<std::string, std::vector<Transition>> transitions;

   // Attach a VoxModel component directly. Note that it doesn't _have_
   // to be hosted by the same entity as this DeprecatedSkeleton.
   Engine::ComponentHandle<VoxModel> model;
};

}; // namespace CubeWorld

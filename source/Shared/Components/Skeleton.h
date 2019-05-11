// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <map>
#include <vector>

#include <RGBBinding/BindingProperty.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Engine/Entity/EntityManager.h>
#include "../Components/VoxModel.h"
#include "../Voxel.h"

namespace CubeWorld
{

struct SkeletonAnimations;

struct Skeleton : Engine::Component<Skeleton>  {
   // Types
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
      std::map<std::string, std::string> parents;
      std::map<std::string, glm::vec3> positions;
      std::map<std::string, glm::vec3> rotations;
      std::map<std::string, glm::vec3> scales;
   };

public:
   static void Transform(glm::mat4& matrix, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

public:
   Skeleton();
   Skeleton(const std::string& path);
   Skeleton(const BindingProperty& data);

   void Reset();
   void Load(const std::string& path);
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

   std::vector<Stance> stances;
   std::unordered_map<std::string, size_t> stanceLookup;

   // Animations to apply to this skeleton.
   Engine::ComponentHandle<SkeletonAnimations> animations;
};

}; // namespace CubeWorld

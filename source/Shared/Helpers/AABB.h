// By Thomas Steinke

#pragma once

#include <algorithm>
#include <glm/glm.hpp>

namespace CubeWorld
{

//
// Axis Aligned Bounding Box.
//
struct AABB
{
   AABB() : min(0), max(0) {};
   AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) {};

   float GetSurfaceArea()
   {
      return 2.0f * (
         (max.x - min.x) * (max.y - min.y) +
         (max.x - min.x) * (max.z - min.z) +
         (max.y - min.y) * (max.z - min.z)
      );
   }

   bool Overlapping(const AABB& other) const
   {
      // Test y first since it's more likely to fail - quicker short circuiting.
      return
         min.y < other.max.y &&
         max.y > other.min.y &&
         min.x < other.max.x &&
         max.x > other.min.x &&
         min.z < other.max.z &&
         max.z > other.min.z;
   }

   bool Contains(const AABB& other) const
   {
      return
         min.x <= other.min.x &&
         min.y <= other.min.y &&
         min.z <= other.min.z &&
         max.x >= other.max.x &&
         max.y >= other.max.y &&
         max.z >= other.max.z;
   }

   AABB Merge(const AABB& other) const
   {
      return AABB(
         glm::vec3(
            std::min(min.x, other.min.x),
            std::min(min.y, other.min.y),
            std::min(min.z, other.min.z)
         ),
         glm::vec3(
            std::max(max.x, other.max.x),
            std::max(max.y, other.max.y),
            std::max(max.z, other.max.z)
         )
      );
   }

   AABB Intersection(const AABB& other) const
   {
      return AABB(
         glm::vec3(
            std::max(min.x, other.min.x),
            std::max(min.y, other.min.y),
            std::max(min.z, other.min.z)
         ),
         glm::vec3(
            std::min(max.x, other.max.x),
            std::min(max.y, other.max.y),
            std::min(max.z, other.max.z)
         )
      );
   }

public:
   glm::vec3 min;
   glm::vec3 max;
};

}; // namespace CubeWorld

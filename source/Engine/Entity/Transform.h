// By Thomas Steinke

#pragma once

#include <glm/ext.hpp>

#include "Component.h"

namespace CubeWorld
{

namespace Engine
{

struct Transform : public Component<Transform> {
   Transform(
      glm::vec3 position,
      glm::vec3 rotation = glm::vec3(1, 1, 1),
      glm::vec3 scale = glm::vec3(1, 1, 1)
   ) 
      : position(position)
      , rotation(rotation)
      , scale(scale)
   {};

   glm::vec3 position;
   glm::vec3 rotation;
   glm::vec3 scale;
};

}; // namespace Engine

}; // namespace CubeWorld

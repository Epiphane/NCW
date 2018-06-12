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
      glm::vec3 direction = glm::vec3(0, 0, 1),
      glm::vec3 up = glm::vec3(0, 1, 0),
      glm::vec3 scale = glm::vec3(1, 1, 1)
   ) 
      : position(position)
      , direction(direction)
      , up(up)
      , scale(scale)
   {};

   glm::vec3 position;
   glm::vec3 direction;
   glm::vec3 up;
   glm::vec3 scale;
};

}; // namespace Engine

}; // namespace CubeWorld

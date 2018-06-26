// By Thomas Steinke

#pragma once

#include <glm/ext.hpp>

#include "Component.h"

namespace CubeWorld
{

namespace Engine
{

class Transform : public Component<Transform> {
public:
   using val = float;

   Transform(
      glm::vec3 position,
      glm::vec3 direction = glm::vec3(0, 0, 1),
      val roll = 0,
      glm::vec3 scale = glm::vec3(1, 1, 1)
   );

   glm::vec3 position;
   glm::vec3 up;
   glm::vec3 scale;

public:
   glm::vec3 GetDirection() const { return direction; }
   val GetPitch() const { return pitch; }
   val GetYaw() const { return yaw; }
   val GetRoll() const { return roll; }

   void SetDirection(glm::vec3 direction);
   void SetPitch(val pitch);
   void SetYaw(val yaw);
   void SetRole(val r) { roll = r; }

private:
   glm::vec3 direction;
   val pitch, yaw, roll;
};

}; // namespace Engine

}; // namespace CubeWorld

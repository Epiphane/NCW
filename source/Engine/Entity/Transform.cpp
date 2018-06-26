// By Thomas Steinke

#include <Engine/Logger/Logger.h>

#include "Transform.h"

namespace CubeWorld
{

namespace Engine
{
   
namespace
{
   
inline glm::vec3 PitchYawToVector(const Transform::val pitch, const Transform::val yaw)
{
   return glm::vec3(sin(yaw), sin(pitch), cos(yaw));
}

inline Transform::val VectorToPitch(glm::vec3 direction)
{
   Transform::val horizontal = sqrt(direction.x*direction.x + direction.z*direction.z);
   return atan2(direction.y, horizontal);
}

inline Transform::val VectorToYaw(glm::vec3 direction)
{
   if (direction.x == 0 && direction.z == 0)
   {
      return 0;
   }
   return atan2(direction.x, direction.z);
}

};

Transform::Transform(
   glm::vec3 position,
   glm::vec3 direction,
   Transform::val roll,
   glm::vec3 scale
) 
   : position(position)
   , scale(scale)
   , direction(direction)
   , pitch(VectorToPitch(direction))
   , yaw(VectorToYaw(direction))
   , roll(roll)
{
};
   
void Transform::SetPitch(Transform::val p)
{
   pitch = p;
   direction = PitchYawToVector(pitch, yaw);
}

void Transform::SetYaw(Transform::val y)
{
   yaw = y;
   direction = PitchYawToVector(pitch, yaw);
}

}; // namespace Engine

}; // namespace CubeWorld

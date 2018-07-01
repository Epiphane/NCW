// By Thomas Steinke

#include <glm/ext.hpp>

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
      float horiz = cos(pitch);
      return glm::vec3(horiz * sin(yaw), sin(pitch), horiz * cos(yaw));
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
   , mDirection(direction)
   , mPitch(VectorToPitch(direction))
   , mYaw(VectorToYaw(direction))
   , mRoll(roll)
{
   mFlatDirection = direction;
   mFlatDirection.y = 0;
   mFlatDirection = glm::normalize(mFlatDirection);
};
   
void Transform::SetPitch(Transform::val p)
{
   mPitch = p;
   mDirection = glm::vec3(0, sin(mPitch), 0) + cos(mPitch) * mFlatDirection;
}

void Transform::SetYaw(Transform::val y)
{
   mYaw = y;
   mFlatDirection = glm::vec3(sin(mYaw), 0, cos(mYaw));
   mDirection = glm::vec3(0, sin(mPitch), 0) + cos(mPitch) * mFlatDirection;
}

}; // namespace Engine

}; // namespace CubeWorld

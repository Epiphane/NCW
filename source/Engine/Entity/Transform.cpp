// By Thomas Steinke

#include <glm/ext.hpp>

#include <Engine/Logger/Logger.h>

#include "EntityManager.h"
#include "Transform.h"

namespace CubeWorld
{

namespace Engine
{
   
namespace
{

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
   : mPosition(position)
   , mScale(scale)
   , mDirection(direction)
   , mPitch(VectorToPitch(direction))
   , mYaw(VectorToYaw(direction))
   , mRoll(roll)
{
   mFlatDirection = direction;
   mFlatDirection.y = 0;
   mFlatDirection = glm::normalize(mFlatDirection);
   ComputeMatrix();
};

void Transform::ComputeMatrix()
{
   mMatrix = glm::mat4(1);

   mMatrix = glm::translate(mMatrix, mPosition);
   mMatrix = glm::rotate(mMatrix, mYaw, glm::vec3(0, 1, 0));
   mMatrix = glm::rotate(mMatrix, mPitch, glm::vec3(1, 0, 0));
   mMatrix = glm::rotate(mMatrix, mRoll, glm::vec3(0, 0, 1));
   mMatrix = glm::scale(mMatrix, mScale);
}

glm::mat4 Transform::GetMatrix() const
{
   if (mParent)
   {
      return mParent->GetMatrix() * mMatrix;
   }
   else
   {
      return mMatrix;
   }
}

glm::vec3 Transform::GetAbsolutePosition() const
{
   glm::vec4 transformed = GetMatrix() * glm::vec4(0, 0, 0, 1);
   return glm::vec3(transformed.x, transformed.y, transformed.z);
   // TODO some fancy stuff without multiplying matrixes all over the place
   // return glm::vec3(mMatrix[3][0], mMatrix[3][1], mMatrix[3][2]);
}

glm::vec3 Transform::GetAbsoluteDirection() const
{
   glm::vec4 transformed = GetMatrix() * glm::vec4(0, 0, 1, 0);
   return glm::vec3(transformed.x, transformed.y, transformed.z);
}

glm::vec3 Transform::GetAbsoluteScale() const
{
   return glm::vec3(mMatrix[0][0], mMatrix[1][1], mMatrix[2][2]);
}

void Transform::SetLocalPosition(glm::vec3 position)
{
   mPosition = position;
   ComputeMatrix();
}

void Transform::SetLocalScale(glm::vec3 scale)
{
   mScale = scale;
   ComputeMatrix();
}

void Transform::SetLocalDirection(glm::vec3 direction)
{
   mDirection = direction;
   mPitch = VectorToPitch(direction);
   mYaw = VectorToYaw(direction);
   mFlatDirection = direction;
   mFlatDirection.y = 0;
   mFlatDirection = glm::normalize(mFlatDirection);
   ComputeMatrix();
}
   
void Transform::SetPitch(Transform::val p)
{
   mPitch = p;
   mDirection = glm::vec3(0, sin(mPitch), 0) + cos(mPitch) * mFlatDirection;
   ComputeMatrix();
}

void Transform::SetYaw(Transform::val y)
{
   mYaw = y;
   mFlatDirection = glm::vec3(sin(mYaw), 0, cos(mYaw));
   mDirection = glm::vec3(0, sin(mPitch), 0) + cos(mPitch) * mFlatDirection;
   ComputeMatrix();
}

}; // namespace Engine

}; // namespace CubeWorld

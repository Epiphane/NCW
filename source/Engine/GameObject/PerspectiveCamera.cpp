// By Thomas Steinke

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <math.h>

#include <Engine/Logger/Logger.h>

#include "PerspectiveCamera.h"

namespace CubeWorld
{

namespace Engine
{

namespace
{

inline glm::vec3 PitchYawToVector(const double pitch, const double yaw)
{
   return glm::vec3(sin(yaw), sin(pitch), cos(yaw));
}

inline double VectorToPitch(glm::vec3 direction)
{
   double horizontal = sqrt(direction.x*direction.x + direction.z*direction.z);
   return atan2(direction.y, horizontal);
}

inline double VectorToYaw(glm::vec3 direction)
{
   return atan2(direction.x, direction.z);
}

};

PerspectiveCamera::PerspectiveCamera(const Options& options) 
   : mPosition(options.position)
   , mDirection(options.direction)
   , mPitch(VectorToPitch(options.direction))
   , mYaw(VectorToYaw(options.direction))
{
   mPerspective = glm::perspective(options.fov, options.aspect, options.near, options.far);
}

glm::mat4 PerspectiveCamera::GetView()
{
   return glm::lookAt(mPosition, mPosition + mDirection, glm::vec3(0, 1, 0));
}

void PerspectiveCamera::SetPosition(glm::vec3 position)
{
   mPosition = position;
}

void PerspectiveCamera::Move(glm::vec3 dPosition)
{
   mPosition += dPosition;
}

void PerspectiveCamera::SetPitch(double pitch)
{
   mPitch = pitch;
   mDirection = PitchYawToVector(mPitch, mYaw);
}

void PerspectiveCamera::SetYaw(double yaw)
{
   mYaw = yaw;
   mDirection = PitchYawToVector(mPitch, mYaw);
}

}; // namespace Engine

}; // namespace CubeWorld
// By Thomas Steinke

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Logger/Logger.h>

#include "RenderCamera.h"

namespace CubeWorld
{

namespace Game
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

namespace Component
{

   RenderCamera::RenderCamera(const Options& options)
      : position(options.position)
      , direction(options.direction)
      , pitch(VectorToPitch(options.direction))
      , yaw(VectorToYaw(options.direction))
   {
      perspective = glm::perspective(options.fov, options.aspect, options.near, options.far);
   }
   
   glm::mat4 RenderCamera::GetView() const
   {
      return glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
   }
   
   void RenderCamera::SetPitch(double p)
   {
      pitch = p;
      direction = PitchYawToVector(pitch, yaw);
   }
   
   void RenderCamera::SetYaw(double y)
   {
      yaw = y;
      direction = PitchYawToVector(pitch, yaw);
   }

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld

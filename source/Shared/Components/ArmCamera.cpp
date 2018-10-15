// By Thomas Steinke

#include <GL/includes.h>
#include <glm/ext.hpp>

#include <Engine/Entity/EntityManager.h>

#include "ArmCamera.h"

namespace CubeWorld
{

ArmCamera::ArmCamera(const Engine::ComponentHandle<Engine::Transform>& transform, const Options& options)
   : transform(transform)
   , aspect(options.aspect)
   , fov(options.fov)
   , near(options.near)
   , far(options.far)
   , distance(options.distance)
   , minDistance(options.minDistance)
   , maxDistance(options.maxDistance)
{
}

glm::mat4 ArmCamera::GetPerspective() const
{
   return glm::perspective(fov, aspect, near, far);
}

glm::mat4 ArmCamera::GetView() const
{
   glm::vec3 pos = transform->GetAbsolutePosition();
   return glm::lookAt(pos - transform->GetAbsoluteDirection() * distance, pos, glm::vec3(0, 1, 0));
}

}; // namespace CubeWorld

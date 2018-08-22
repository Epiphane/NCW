// By Thomas Steinke

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Entity/EntityManager.h>

#include "ArmCamera.h"

namespace CubeWorld
{

namespace Game
{

ArmCamera::ArmCamera(const Engine::ComponentHandle<Engine::Transform>& transform, const Options& options)
   : transform(transform)
   , distance(options.distance)
   , minDistance(options.minDistance)
   , maxDistance(options.maxDistance)
{
   perspective = glm::perspective(options.fov, options.aspect, options.near, options.far);
}

glm::mat4 ArmCamera::GetView() const
{
   glm::vec3 pos = transform->GetAbsolutePosition();
   return glm::lookAt(pos - transform->GetAbsoluteDirection() * distance, pos, glm::vec3(0, 1, 0));
}
   
}; // namespace Game

}; // namespace CubeWorld

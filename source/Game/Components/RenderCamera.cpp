// By Thomas Steinke

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Entity/EntityManager.h>

#include "RenderCamera.h"

namespace CubeWorld
{

namespace Game
{

namespace Component
{

   RenderCamera::RenderCamera(const Engine::ComponentHandle<Engine::Transform>& transform, const Options& options)
      : transform(transform)
   {
      perspective = glm::perspective(options.fov, options.aspect, options.near, options.far);
   }

   glm::mat4 RenderCamera::GetView() const
   {
      return glm::lookAt(transform->position, transform->position + transform->GetDirection(), glm::vec3(0, 1, 0));
   }

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld

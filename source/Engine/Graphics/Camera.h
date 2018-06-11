// By Thomas Steinke

#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

   class Camera
   {
   public:
      virtual glm::mat4 GetPerspective() const = 0;
      virtual glm::mat4 GetView() const = 0;
   };
   
}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld

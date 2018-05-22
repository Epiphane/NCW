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
   virtual glm::mat4 GetPerspective() = 0;
   virtual glm::mat4 GetView() = 0;
};

class PerspectiveCamera : public Camera
{
public:
   PerspectiveCamera(
      glm::vec3 position,
      glm::vec3 direction,
      const double aspect,
      const double fov = 45.0,
      const double near = 0.1,
      const double far = 100.0
   );

   glm::mat4 GetPerspective() override { return mPerspective; }
   glm::mat4 GetView() override;

private:
   glm::vec3 mPosition;
   glm::vec3 mDirection;
   glm::mat4 mPerspective;
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld

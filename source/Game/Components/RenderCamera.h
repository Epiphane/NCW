// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Engine/Graphics/Camera.h>
#include <Engine/Entity/Component.h>

namespace CubeWorld
{

namespace Game
{

namespace Component
{
   
   class RenderCamera : public Engine::Component<RenderCamera>, public Engine::Graphics::Camera {
   public:
      //
      // Options for initializing a RenderCamera. Most of them define the perspective matrix.
      //
      struct Options
      {
         glm::vec3 position;
         glm::vec3 direction;
         double aspect;
         double fov = 45.0;
         double near = 0.1;
         double far = 100.0;
      };
      
      RenderCamera(const Options& options);
      
      glm::mat4 GetPerspective() const override { return perspective; }
      glm::mat4 GetView() const override;
      
      void SetPitch(double pitch);
      void SetYaw(double yaw);
      
      glm::vec3 position;
      glm::vec3 direction;
      glm::mat4 perspective;
      
      double pitch, yaw;
   };

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld

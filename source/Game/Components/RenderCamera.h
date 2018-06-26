// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Engine/Graphics/Camera.h>
#include <Engine/Entity/Component.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Engine/Entity/Transform.h>

namespace CubeWorld
{

namespace Game
{

namespace Component
{
   
   //
   // RenderCamera implements the Camera class in a unique way.
   //
   // Perspective is pretty straightforward, just a set of options, but it depends on
   // a direct link to an entity's Transform component as well.
   //
   class RenderCamera : public Engine::Component<RenderCamera>, public Engine::Graphics::Camera {
   public:
      //
      // Options for initializing a RenderCamera. Most of them define the perspective matrix.
      //
      struct Options
      {
         double aspect;
         double fov = 45.0;
         double near = 0.1;
         double far = 100.0;
      };
      
      RenderCamera(const Engine::ComponentHandle<Engine::Transform>& transform, const Options& options);
      ~RenderCamera() {};
      
      glm::mat4 GetPerspective() const override { return perspective; }
      glm::mat4 GetView() const override;
      
      glm::mat4 perspective;

      Engine::ComponentHandle<Engine::Transform> transform;
   };

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld

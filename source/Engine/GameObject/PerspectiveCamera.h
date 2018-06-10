// By Thomas Steinke

#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../Core/State.h"
#include <Engine/Graphics/Camera.h>

namespace CubeWorld
{

namespace Engine
{

class PerspectiveCamera : public Graphics::Camera//, public GeneralComponent
{
public:
   struct Options
   {
      //
      // Initial position.
      //
      glm::vec3 position;

      //
      // Initial direction.
      //
      glm::vec3 direction;

      //
      // Aspect ratio of the window.
      //
      double aspect;

      //
      // Field of view angle
      //
      double fov = 45.0;

      //
      // Near culling distance.
      //
      double near = 0.1;

      //
      // Far culling distance.
      //
      double far = 100.0;
   };

   PerspectiveCamera(const Options& options);

   glm::mat4 GetPerspective() override { return mPerspective; }
   glm::mat4 GetView() override;

   void Update(const State* /*world*/, const Input::InputManager* /*input*/, double /*dt*/, GameObject* /*gameObject*/) {}

   void SetPosition(glm::vec3 position);
   void Move(glm::vec3 dPosition);
   void SetPitch(double pitch);
   void SetYaw(double yaw);

   glm::vec3 GetPosition() { return mPosition; }
   double GetPitch() { return mPitch; }
   double GetYaw() { return mYaw; }

private:
   glm::vec3 mPosition;
   glm::vec3 mDirection;
   glm::mat4 mPerspective;

   double mPitch, mYaw;
};

}; // namespace Engine

}; // namespace CubeWorld

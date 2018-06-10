// By Thomas Steinke

#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Engine/GameObject/PerspectiveCamera.h>

namespace CubeWorld
{

namespace Game
{

namespace Component
{

class MousePerspectiveCamera : public Engine::PerspectiveCamera
{
public:
   struct Options : Engine::PerspectiveCamera::Options
   {
      //
      // Pitch sensitivity, for vertical movement.
      //
      double pitchSensitivity = 0.007;

      //
      // Yaw sensitivity, for horizontal movement.
      //
      double yawSensitivity = 0.007;
   };

   MousePerspectiveCamera(const Options& options);

   void Update(const Engine::State* world, const Engine::Input::InputManager* input, double dt, Engine::GameObject* /*gameObject*/);

private:
   double mSensitivity[2];
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld

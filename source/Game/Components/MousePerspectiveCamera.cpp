// By Thomas Steinke

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Logger/Logger.h>

#include "MousePerspectiveCamera.h"

namespace CubeWorld
{

namespace Game
{

namespace Component
{

MousePerspectiveCamera::MousePerspectiveCamera(const Options& options)
   : PerspectiveCamera(options)
   , mSensitivity{options.yawSensitivity, options.pitchSensitivity}
{
}

void MousePerspectiveCamera::Update(const Engine::State* /*world*/, const Engine::Input::InputManager* input, double /*dt*/, Engine::GameObject* /*gameObject*/)
{
   double movement[2];
   input->GetMouse(nullptr, movement);

   SetYaw(GetYaw() + mSensitivity[0] * movement[0]);
   SetPitch(GetPitch() + mSensitivity[1] * movement[1]);
}

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
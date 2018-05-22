// By Thomas Steinke

#pragma once

#include "StupidInputComponent.h"

namespace CubeWorld
{

namespace Game
{

namespace Component
{

void StupidInputComponent::Update(
   const Engine::State* /* world */,
   const Engine::Input::InputManager* input,
   Engine::GameObject* gameObject)
{
   if (input->IsKeyDown(GLFW_KEY_W))
   {
      gameObject->SetPosition(gameObject->GetPosition() - glm::vec3(0.1, 0, 0));
   }

   if (input->IsKeyDown(GLFW_KEY_RIGHT))
   {
      gameObject->SetPosition(gameObject->GetPosition() + glm::vec3(0.1, 0, 0));
   }
}

}; // namespace Component

}; // namespace Game

}; // namespace CubeWorld

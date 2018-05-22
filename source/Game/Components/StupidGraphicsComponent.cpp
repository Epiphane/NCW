// By Thomas Steinke

#pragma once

#include <glm/ext.hpp>

#include "StupidGraphicsComponent.h"

namespace CubeWorld
{

namespace Game
{

namespace Component
{

void StupidGraphicsComponent::Render(
   const glm::mat4& perspective,
   const glm::mat4& view,
   const glm::mat4& /* transform */,
   Engine::GameObject* object
)
{
   glm::mat4 transform = glm::translate(glm::mat4(1), object->GetPosition());

   mRenderer.Render(perspective, view, transform);
}

}; // namespace Component

}; // namespace Game

}; // namespace CubeWorld

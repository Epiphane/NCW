// By Thomas Steinke

#pragma once

#include <string>

#include <glm/glm.hpp>

#include <Engine/Core/State.h>
#include <Engine/Core/Input.h>

namespace CubeWorld
{

namespace Engine
{

namespace Component
{

class GraphicsComponent {
public:
   virtual void Render(
      const glm::mat4& perspective,
      const glm::mat4& view,
      const glm::mat4& transform,
      GameObject* object
   ) = 0;
};

}; // namespace Component

}; // namespace Engine

}; // namespace CubeWorld

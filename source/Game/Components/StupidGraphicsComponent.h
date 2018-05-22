// By Thomas Steinke

#pragma once

#include <Engine/GameObject/GameObject.h>
#include <Engine/GameObject/GraphicsComponent.h>

#include "../Renderers/StupidRenderer.h"

namespace CubeWorld
{

namespace Game
{

namespace Component
{

class StupidGraphicsComponent : public Engine::Component::GraphicsComponent {
public:
   void Render(
      const glm::mat4& perspective,
      const glm::mat4& view,
      const glm::mat4& transform,
      Engine::GameObject* object
   ) override;

private:

   Game::StupidRenderer mRenderer;
};

}; // namespace Component

}; // namespace Game

}; // namespace CubeWorld

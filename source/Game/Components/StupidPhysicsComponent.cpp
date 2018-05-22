// By Thomas Steinke

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Logger/Logger.h>
#include <Engine/GameObject/GameObject.h>

#include "StupidPhysicsComponent.h"

namespace CubeWorld
{

namespace Game
{

namespace Component
{

void StupidPhysicsComponent::Update(const Engine::State* /*world*/, const Engine::Input::InputManager* input, double dt, Engine::GameObject* gameObject)
{
   gameObject->SetPosition(gameObject->GetPosition() + mVelocity * (float) dt);
}

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
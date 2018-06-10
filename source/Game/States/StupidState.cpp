// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <Engine/Core/Input.h>
#include <Engine/Logger/Logger.h>

#include "StupidState.h"

namespace CubeWorld
{

namespace Game
{

void StupidState::Start()
{
   /*Engine::Input::InputManager::Instance()->SetMouseLock(true);

   Component::MousePerspectiveCamera::Options cameraOptions;
   cameraOptions.position = glm::vec3(0, 0, 1);
   cameraOptions.direction = glm::vec3(0, 0, -1);
   cameraOptions.aspect = 16.0 / 9.0;
   std::unique_ptr<Component::MousePerspectiveCamera> camera = std::make_unique<Component::MousePerspectiveCamera>(cameraOptions);

   mCamera = camera.get();

   Engine::GameObject::Options playerOptions;
   playerOptions.inputComponent = std::make_unique<Component::StupidInputComponent>();
   playerOptions.components[playerOptions.numComponents++] = std::move(camera);
   playerOptions.components[playerOptions.numComponents++] = std::make_unique<Component::StupidPhysicsComponent>();

   AddObject(std::make_unique<Engine::GameObject>(playerOptions));

   Engine::GameObject::Options triangleOptions;
   triangleOptions.inputComponent = std::make_unique<Component::StupidInputComponent>();
   triangleOptions.graphicsComponent = std::make_unique<Component::StupidGraphicsComponent>();

   AddObject(std::make_unique<Engine::GameObject>(triangleOptions));*/
}

}; // namespace Game

}; // namespace CubeWorld
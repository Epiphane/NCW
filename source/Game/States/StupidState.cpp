// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <Engine/GameObject/PerspectiveCamera.h>
#include <Game/Systems/Simple3DRenderSystem.h>

#include "StupidState.h"

namespace CubeWorld
{

namespace Game
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

StupidState::StupidState()
{
   Engine::PerspectiveCamera::Options cameraOptions;
   cameraOptions.position = glm::vec3(0, 0, 1);
   cameraOptions.direction = glm::vec3(0, 0, -1);
   cameraOptions.aspect = 16.0 / 9.0;
   mCam = std::make_unique<Engine::PerspectiveCamera>(cameraOptions);

   mSystems.Add<Simple3DRenderSystem>(mCam.get());
}

StupidState::~StupidState()
{

}

void StupidState::Start()
{
   // Create some stupid component.
   Entity triangle = mEntities.Create();
   triangle.Add<Transform>(glm::vec3(0, 0, 0));
   triangle.Add<Simple3DRender>(std::vector<GLfloat>{
      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       0.0f, 1.0f, 0.0f, 
   }, std::vector<GLfloat> {
      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
   });

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
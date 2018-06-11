// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <Game/Components/RenderCamera.h>
#include <Game/Systems/Simple3DRenderSystem.h>
#include <Game/Systems/CameraSystem.h>

#include "StupidState.h"

namespace CubeWorld
{

namespace Game
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

StupidState::StupidState()
{
   mSystems.Add<Simple3DRenderSystem>();
   mSystems.Add<CameraSystem>(Engine::Input::InputManager::Instance());
   
   mSystems.Configure();
}

StupidState::~StupidState()
{

}

void StupidState::Start()
{
   Engine::Input::InputManager::Instance()->SetMouseLock(true);
   
   // Create some stupid component.
   Entity triangle = mEntities.Create();
   triangle.Add<Transform>(glm::vec3(-100, 0, 0));
   triangle.Add<Simple3DRender>(std::vector<GLfloat>{
      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
   }, std::vector<GLfloat> {
       0.0f, 0.0f, 1.0f,
       1.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
   });
   
   for (int i = -100; i < 100; ++i)
   {
      Entity triangle2 = mEntities.Create();
      triangle2.Add<Transform>(glm::vec3(i + 1, 0, 0));
      triangle2.Add<Simple3DRender>(*triangle.Get<Simple3DRender>());
   }
   
   Entity player = mEntities.Create();
   player.Add<Transform>(glm::vec3(0, 0, 0));
   Component::RenderCamera::Options cameraOptions;
   cameraOptions.position = glm::vec3(0, 0, 10);
   cameraOptions.direction = glm::vec3(0, 0, -1);
   cameraOptions.aspect = 1280.0 / 800.0;
   Engine::ComponentHandle<Component::RenderCamera> handle = player.Add<Component::RenderCamera>(cameraOptions);
   player.Add<MouseControlledCamera>();
   
   mSystems.Get<Simple3DRenderSystem>()->SetCamera(handle.get());
}

}; // namespace Game

}; // namespace CubeWorld

// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <Engine/Entity/Transform.h>
#include <Game/Components/RenderCamera.h>
#include <Game/Systems/Simple3DRenderSystem.h>
#include <Game/Systems/CameraSystem.h>
#include <Game/Systems/FlySystem.h>

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
   mSystems.Add<FlySystem>(Engine::Input::InputManager::Instance());
   
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
   triangle.Add<Transform>(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0));
   triangle.Add<Simple3DRender>(std::vector<GLfloat>{
      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f,
   }, std::vector<GLfloat> {
       0.0f, 0.0f, 1.0f,
       1.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
   });
   
   for (double i = 0; i < 2 * 3.14159; i += 3.14159/32)
   {
      Entity triangle2 = mEntities.Create();
      glm::vec3 pos = glm::vec3(sin(i), 0, cos(i));
      triangle2.Add<Transform>(10.0f * pos, -pos);
      triangle2.Add<Simple3DRender>(*triangle.Get<Simple3DRender>());

      Entity triangle3 = mEntities.Create();
      triangle3.Add<Transform>(10.0f * pos - glm::vec3(0, 1 + i/1000, 0), glm::vec3(0, 1, 0), -i);
      triangle3.Add<Simple3DRender>(*triangle.Get<Simple3DRender>());
   }
   
   Entity player = mEntities.Create();
   player.Add<Transform>(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
   Component::RenderCamera::Options cameraOptions;
   cameraOptions.aspect = 1280.0 / 800.0;
   Engine::ComponentHandle<Component::RenderCamera> handle = player.Add<Component::RenderCamera>(
      player.Get<Transform>(),
      cameraOptions
   );
   player.Add<MouseControlledCamera>();
   player.Add<FlySpeed>(3);
   
   mSystems.Get<Simple3DRenderSystem>()->SetCamera(handle.get());
}

}; // namespace Game

}; // namespace CubeWorld

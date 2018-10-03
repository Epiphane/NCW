// By Thomas Steinke

#include <cassert>
#include <cstdlib>
#include <functional>

#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Shared/Components/CubeModel.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>

#include "../Systems/AnimationSystem.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

using ArmCamera = Game::ArmCamera;
using DebugHelper = Game::DebugHelper;

MainState::MainState(Engine::Window* window, Bounded& parent)
   : mWindow(window)
   , mParent(parent)
{
}

MainState::~MainState()
{
   DebugHelper::Instance()->SetSystemManager(nullptr);
}

void MainState::Initialize()
{
   // Create systems and configure
   DebugHelper::Instance()->SetSystemManager(&mSystems);
   mSystems.Add<Game::CameraSystem>(mWindow->GetInput());
   mSystems.Add<Game::VoxelRenderSystem>(&mCamera);
   mSystems.Configure();

   // Unlock the mouse
   mWindow->GetInput()->SetMouseLock(false);

   // Add a shell entity for controlling animation state
   Entity controls = mEntities.Create();
   controls.Add<AnimationSystemController>();

   Entity voxels = mEntities.Create(0, 0, 0);
   mBackdrop = std::make_unique<Backdrop>(voxels.Add<Game::VoxelRender>());
   mEvents.Subscribe<ModelLoadedEvent>(*mBackdrop);

   // Create a player component
   Entity player = mEntities.Create();
   player.Add<Transform>(glm::vec3(0));
   player.Add<Game::CubeModel>(Asset::Model("dummy.cub"));

   // Create a camera
   Entity playerCamera = mEntities.Create(0, 0, 0);
   playerCamera.Get<Transform>()->SetParent(player);
   playerCamera.Get<Transform>()->SetLocalScale(glm::vec3(10.0));
   playerCamera.Get<Transform>()->SetLocalDirection(glm::vec3(1, 0.5, -1));
   ArmCamera::Options cameraOptions;
   cameraOptions.aspect = float(mParent.GetWidth()) / mParent.GetHeight();
   cameraOptions.far = 1500.0f;
   cameraOptions.distance = 2.25f;
   cameraOptions.minDistance = 0.5f;
   Engine::ComponentHandle<ArmCamera> handle = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
   playerCamera.Add<Game::KeyControlledCamera>();
   playerCamera.Add<Game::MouseControlledCameraArm>();

   mCamera.Set(handle.get());
}

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld

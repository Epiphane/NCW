// By Thomas Steinke

#include <cassert>
#include <cstdlib>
#include <functional>

#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Shared/Components/CubeModel.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

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

using ArmCamera = ArmCamera;
using DebugHelper = DebugHelper;

MainState::MainState(Engine::Input* input, Bounded& parent)
   : mInput(input)
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
   mSystems.Add<CameraSystem>(mInput);
   mSystems.Add<VoxelRenderSystem>(&mCamera);
   mSystems.Configure();

   // Unlock the mouse
   mInput->SetMouseLock(false);

   Entity voxels = mEntities.Create(0, 0, 0);
   mBackdrop = std::make_unique<Backdrop>(voxels.Add<VoxelRender>());
   mEvents.Subscribe<ModelLoadedEvent>(*mBackdrop);

   // Create a player component
   Entity player = mEntities.Create();
   player.Add<Transform>(glm::vec3(0));
   player.Add<CubeModel>(Asset::Model("dummy.cub"));

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
   mPlayerCam = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
   playerCamera.Add<KeyControlledCamera>();
   playerCamera.Add<MouseControlledCameraArm>();

   mCamera.Set(mPlayerCam.get());

   mEvents.Subscribe<Engine::UIRebalancedEvent>(*this);
}

void MainState::Receive(const Engine::UIRebalancedEvent&)
{
   mPlayerCam->aspect = float(mParent.GetWidth()) / mParent.GetHeight();
}

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld

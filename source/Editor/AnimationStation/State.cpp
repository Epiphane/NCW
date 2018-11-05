// By Thomas Steinke

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <functional>
#pragma warning(push, 0)
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>
#pragma warning(pop)

#include <Engine/Core/Paths.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Engine/System/InputEventSystem.h>
#include <Shared/Components/CubeModel.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/SimplePhysicsSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "../Systems/AnimationSystem.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

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
   mSystems.Add<CameraSystem>(mWindow->GetInput());
   mSystems.Add<Editor::AnimationSystem>();
   mSystems.Add<MakeshiftSystem>();
   mSystems.Add<VoxelRenderSystem>(&mCamera);
   mSystems.Configure();

   // Unlock the mouse
   mWindow->GetInput()->SetMouseLock(false);

   // Add a shell entity for controlling animation state
   Entity controls = mEntities.Create();
   controls.Add<AnimationSystemController>();

   // Create a player component
   Entity player = mEntities.Create();
   player.Add<Transform>(glm::vec3(0, 1.3, 0));
   player.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
   player.Add<AnimatedSkeleton>();

   // Create a camera
   Entity playerCamera = mEntities.Create(0, 0, 0);
   playerCamera.Get<Transform>()->SetParent(player);
   playerCamera.Get<Transform>()->SetLocalScale(glm::vec3(10.0));
   playerCamera.Get<Transform>()->SetLocalDirection(glm::vec3(1, 0.5, -1));
   ArmCamera::Options cameraOptions;
   cameraOptions.aspect = float(mParent.GetWidth()) / mParent.GetHeight();
   cameraOptions.far = 1500.0f;
   cameraOptions.distance = 3.5f;
   mPlayerCam = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
   playerCamera.Add<KeyControlledCamera>();
   playerCamera.Add<KeyControlledCameraArm>();

   mCamera.Set(mPlayerCam.get());

   // Add some voxels.
   std::vector<Voxel::Data> carpet;
   std::vector<glm::vec3> points;
   std::vector<glm::vec3> colors;

   // Colors
   const glm::vec4 BASE(18, 18, 18, 1);
   const glm::vec4 LINE(0, 160, 51, 1);
   const int size = 150;
   for (int i = -size; i <= size; ++i) {
      for (int j = -size; j <= size; ++j) {
         double x = (double)i / (2 * size);
         double y = (double)j / (2 * size);
         double expectedX = 1.0 - 4.0 * std::pow(y - 0.5, 2);
         double dist = 5.0 * std::abs(x - expectedX);
         dist += (rand() % 500) / 1000.0 - 0.25;
         // Curve it from (0, 1)
         dist = 1.0 / (1 + std::pow(2, dist));

         glm::vec4 color(
            std::floor((1 - dist) * BASE.r + dist * LINE.r),
            std::floor((1 - dist) * BASE.g + dist * LINE.g),
            std::floor((1 - dist) * BASE.b + dist * LINE.b),
            1);

         glm::vec3 position = glm::vec3(i, 0, j);
         carpet.push_back(Voxel::Data(position, color, Voxel::Top));
      }
   }

   assert(carpet.size() > 0);

   Entity voxels = mEntities.Create(0, 0, 0);
   voxels.Add<VoxelRender>(std::move(carpet));

   mEvents.Subscribe<Engine::UIRebalancedEvent>(*this);
}

void MainState::Receive(const Engine::UIRebalancedEvent&)
{
   mPlayerCam->aspect = float(mParent.GetWidth()) / mParent.GetHeight();
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

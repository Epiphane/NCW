// By Thomas Steinke

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <functional>
#pragma warning(push, 0)
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>
#pragma warning(pop)

#include <RGBFileSystem/Paths.h>
#include <Engine/Core/StateManager.h>
#include <RGBLogger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Engine/System/InputEventSystem.h>
#include <Shared/Components/VoxModel.h>
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

MainState::MainState(Engine::Input* input, Bounded& parent)
   : mInput(input)
   , mParent(parent)
   , mPlayer(&mEntities, Engine::Entity::ID(0))
{
}

MainState::~MainState()
{
   DebugHelper::Instance()->SetSystemManager(nullptr);
}

namespace
{

int32_t CalculateHeight(int x, int y)
{
   double dist = x + std::sin(y / 20.0);// std::sqrt(x * x + y * y);

   //1 / (1 + 2 ^ ((x - 15) / 3))
   return int32_t(std::floor(11.0 / (1.0 + std::pow(2, (dist - 15.0) / 3.0))) - 10);
}

}; // anonymous namespace

void MainState::Initialize()
{
   mEvents.Subscribe<Engine::UIRebalancedEvent>(*this);
   mEvents.Subscribe<SkeletonClearedEvent>(*this);
   mEvents.Subscribe<AddSkeletonPartEvent>(*this);

   // Create systems and configure
   DebugHelper::Instance()->SetSystemManager(&mSystems);
   mSystems.Add<CameraSystem>(mInput);
   mSystems.Add<Editor::AnimationSystem>();
   mSystems.Add<MakeshiftSystem>();
   mSystems.Add<VoxelRenderSystem>(&mCamera);
   mSystems.Configure();

   // Unlock the mouse
   mInput->SetMouseLock(false);

   // Add a shell entity for controlling animation state
   Entity controls = mEntities.Create();
   controls.Add<AnimationSystemController>();

   // Create a player component
   mPlayer = mEntities.Create();
   mPlayer.Add<Transform>(glm::vec3(0, 1.3, 0));
   mPlayer.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
   mPlayer.Add<AnimationController>();

   // Create a camera
   Entity playerCamera = mEntities.Create(0, 2, 0);
   playerCamera.Get<Transform>()->SetParent(mPlayer);
   playerCamera.Get<Transform>()->SetLocalScale(glm::vec3(10.0));
   playerCamera.Get<Transform>()->SetLocalDirection(glm::vec3(1, 0.5, -1));
   ArmCamera::Options cameraOptions;
   cameraOptions.aspect = float(mParent.GetWidth()) / mParent.GetHeight();
   cameraOptions.far = 1500.0f;
   cameraOptions.distance = 3.5f;
   mPlayerCam = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
   playerCamera.Add<MouseDragCamera>(GLFW_MOUSE_BUTTON_LEFT);
   playerCamera.Add<MouseControlledCameraArm>();

   mCamera.Set(mPlayerCam.get());

   // Add some voxels.
   std::vector<Voxel::Data> carpet;
   std::vector<glm::vec3> points;
   std::vector<glm::vec3> colors;

   // Colors
   const glm::vec4 BASE(24, 109, 0, 1);
   const glm::vec4 LINE(105, 157, 3, 1);
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
         dist = std::sin(x * 20);// -std::floor(x * 20);

         glm::vec4 color(
            std::floor((1 - dist) * BASE.r + dist * LINE.r),
            std::floor((1 - dist) * BASE.g + dist * LINE.g),
            std::floor((1 - dist) * BASE.b + dist * LINE.b),
            1);

         glm::vec3 position = glm::vec3(i, CalculateHeight(i, j), j);
         uint8_t sides = Voxel::Top;
         if (CalculateHeight(i - 1, j) < position.y)
         {
            sides |= Voxel::Left;
         }
         if (CalculateHeight(i + 1, j) < position.y)
         {
            sides |= Voxel::Right;
         }
         if (CalculateHeight(i, j + 1) < position.y)
         {
            sides |= Voxel::Front;
         }
         if (CalculateHeight(i, j - 1) < position.y)
         {
            sides |= Voxel::Back;
         }

         carpet.push_back(Voxel::Data(position, color, sides));
      }
   }

   assert(carpet.size() > 0);

   Entity voxels = mEntities.Create(0, 0, 0);
   voxels.Add<VoxelRender>(std::move(carpet));
}

void MainState::Receive(const Engine::UIRebalancedEvent&)
{
   mPlayerCam->aspect = float(mParent.GetWidth()) / mParent.GetHeight();
}

void MainState::Receive(const SkeletonClearedEvent&)
{
   for (const Engine::Entity& part : mPlayerParts)
   {
      mEntities.Destroy(part.GetID());
   }

   auto controller = mPlayer.Get<AnimationController>();
   controller->Reset();

   mPlayerParts.clear();
}

void MainState::Receive(const AddSkeletonPartEvent& evt)
{
   Engine::Entity part = mEntities.Create(0, 0, 0);
   part.Get<Transform>()->SetParent(mPlayer);
   auto model = part.Add<VoxModel>();
   auto skeleton = part.Add<AnimatedSkeleton>(evt.filename, model);

   auto controller = mPlayer.Get<AnimationController>();
   controller->AddSkeleton(skeleton);

   mPlayerParts.push_back(part);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

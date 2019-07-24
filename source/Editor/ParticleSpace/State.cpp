// By Thomas Steinke

#include <cassert>
#include <cstdlib>
#include <functional>

#include <RGBFileSystem/Paths.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <Engine/Core/StateManager.h>
#include <RGBLogger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Engine/System/InputEventSystem.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/SimpleParticleSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

MainState::MainState(Engine::Input* input, Bounded& parent)
   : mParticleSpawner(&mEntities, Engine::Entity::ID(0))
   , mInput(input)
   , mParent(parent)
{
}

MainState::~MainState()
{
   DebugHelper::Instance().SetSystemManager(nullptr);
}

void MainState::Initialize()
{
   mEvents.Subscribe<Engine::UIRebalancedEvent>(*this);
   mEvents.Subscribe<ClearParticleEmitterEvent>(*this);
   mEvents.Subscribe<LoadParticleEmitterEvent>(*this);

   // Create systems and configure
   DebugHelper::Instance().SetSystemManager(&mSystems);
   mSystems.Add<CameraSystem>(mInput);
   mSystems.Add<MakeshiftSystem>();
   mSystems.Add<SimpleParticleSystem>(&mCamera);
   mSystems.Add<VoxelRenderSystem>(&mCamera);
   mSystems.Configure();

   // Unlock the mouse
   mInput->SetMouseLock(false);

   // Create a player component
   mParticleSpawner = mEntities.Create(0, 0, 0);

   // Create a camera
   Engine::Entity playerCamera = mEntities.Create(0, 0, 0);
   playerCamera.Get<Engine::Transform>()->SetParent(mParticleSpawner);
   playerCamera.Get<Engine::Transform>()->SetLocalDirection(glm::vec3(1, 0.5, -1));
   ArmCamera::Options cameraOptions;
   cameraOptions.aspect = float(mParent.GetWidth()) / mParent.GetHeight();
   cameraOptions.far = 1500.0f;
   cameraOptions.distance = 3.5f;
   cameraOptions.minDistance = 1;
   mPlayerCam = playerCamera.Add<ArmCamera>(playerCamera.Get<Engine::Transform>(), cameraOptions);
   playerCamera.Add<MouseDragCamera>(GLFW_MOUSE_BUTTON_LEFT);
   playerCamera.Add<MouseControlledCameraArm>();

   mCamera.Set(mPlayerCam.get());

   // Add some voxels.
   std::vector<Voxel::Data> carpet;
   std::vector<glm::vec3> points;
   std::vector<glm::vec3> colors;

   // Colors
   const glm::vec4 BASE(18, 18, 18, 1);
   const glm::vec4 LINE(157, 3, 3, 1);
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

   Engine::Entity voxels = mEntities.Create(0, -2, 0);
   voxels.Add<VoxelRender>(std::move(carpet));

   mEvents.Emit<ParticleEmitterReadyEvent>();
}

void MainState::Receive(const Engine::UIRebalancedEvent&)
{
   mPlayerCam->aspect = float(mParent.GetWidth()) / mParent.GetHeight();
}

void MainState::Receive(const ClearParticleEmitterEvent&)
{
   if (mParticleSpawner.Has<ParticleEmitter>())
   {
      mParticleSpawner.Remove<ParticleEmitter>();
   }
}

void MainState::Receive(const LoadParticleEmitterEvent& evt)
{
   LOG_DEBUG("Loading emitter %1", evt.filename);
   Maybe<BindingProperty> data = YAMLSerializer::DeserializeFile(evt.filename);
   if (!data)
   {
      data.Failure().WithContext("Failed loading %1", evt.filename).Log();
      return;
   }

   Receive(ClearParticleEmitterEvent{});
   mParticleSpawner.Add<ParticleEmitter>(
      Paths::Join(Paths::GetDirectory(evt.filename), "shaders"),
      *data
   );
}

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld

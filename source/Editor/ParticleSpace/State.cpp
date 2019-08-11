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
#include "../Systems/EditorBackdropSystem.h"
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
   mSystems.Add<EditorBackdropSystem>(&mCamera);
   mSystems.Add<SimpleParticleSystem>(&mCamera);
   mSystems.Add<VoxelRenderSystem>(&mCamera);
   mSystems.Configure();

   // Unlock the mouse
   mInput->SetMouseLock(false);

   // Create a player component
   mParticleSpawner = mEntities.Create(0, 0, 0);
   mParticleSpawner.Add<Makeshift>([&](Engine::EntityManager&, Engine::EventManager&, TIMEDELTA) {
      Engine::ComponentHandle<ParticleEmitter> emitter = mParticleSpawner.Get<ParticleEmitter>();
      if (!emitter)
      {
         mParticleSpawner.Get<Engine::Transform>()->SetLocalPosition({0, 0, 0});
         return;
      }

      // Reset emitter
      if (emitter->age > emitter->emitterLifetime + emitter->particleLifetime + 1.0f)
      {
         emitter->Reset();
      }

      if (emitter->shape != ParticleEmitter::Shape::Trail || emitter->emitterLifetime == 0.0f)
      {
         mParticleSpawner.Get<Engine::Transform>()->SetLocalPosition({0, 0, 0});
         return;
      }

      mParticleSpawner.Get<Engine::Transform>()->SetLocalPosition({
         std::fmodf(emitter->age, emitter->emitterLifetime) / emitter->emitterLifetime,
         0,
         0,
      });
   });

   // Create a camera
   Engine::Entity playerCamera = mEntities.Create(0, 0, 0);
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

   // Add floor and wireframe
   Engine::Entity entity = mEntities.Create(0, 0, 0);
   entity.Add<EditorWireframe>();

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
   Receive(ClearParticleEmitterEvent{});
   mParticleSpawner.Add<ParticleEmitter>(evt.filename);
}

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld

// By Thomas Steinke

#include <cassert>
#include <cstdlib>
#include <functional>

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Entity/Transform.h>
#include <Engine/System/InputEventSystem.h>
#include <Shared/Components/VoxModel.h>
#include <Shared/Helpers/Noise.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/SimpleParticleSystem.h>
#include <Shared/Systems/SimplePhysicsSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "../Systems/EditorBackdropSystem.h"
#include "AnimationDebugSystem.h"
#include "SimpleAnimationSystem.h"
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
   : mPlayer(&mEntities, Engine::Entity::ID(0, 0))
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
   mEvents.Subscribe<SkeletonClearedEvent>(*this);
   mEvents.Subscribe<AddSkeletonPartEvent>(*this);

   // Create systems and configure
   DebugHelper::Instance().SetSystemManager(&mSystems);
   mSystems.Add<CameraSystem>(mInput);
   mSystems.Add<SimpleAnimationSystem>();
   mSystems.Add<AnimationDebugSystem>(true, &mCamera);
   mSystems.Add<VoxelRenderSystem>(&mCamera);
   mSystems.Add<SimpleParticleSystem>(&mCamera);
   mSystems.Add<MakeshiftSystem>();
   mSystems.Configure();

   // Unlock the mouse
   mInput->SetMouseLock(false);

   // Add a shell entity for controlling animation state
   Entity controls = mEntities.Create();
   controls.Add<AnimationSystemController>();

   // Create a player component
   mPlayer = mEntities.Create();
   mPlayer.Add<Transform>(glm::vec3(0, 0.3, 0));
   mPlayer.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
   mPlayer.Add<SimpleAnimationController>(
      mPlayer.Add<MultipleParticleEmitters>()
   );

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

   playerCamera.Add<Makeshift>([&]{
      mPlayerCam->aspect = float(mParent.GetWidth()) / mParent.GetHeight();
   });

   // Add some voxels.
   std::vector<Voxel::Data> carpet;
   std::vector<glm::vec3> points;
   std::vector<glm::vec3> colors;

   Maybe<void> floor = AddFloor(mEntities, glm::vec3(105, 157, 3));
}

void MainState::Receive(const SkeletonClearedEvent&)
{
   for (const Engine::Entity& part : mPlayerParts)
   {
      mEntities.Destroy(part.GetID());
   }

   auto controller = mPlayer.Get<SimpleAnimationController>();
   controller->Reset();

   mPlayerParts.clear();
}

void MainState::Receive(const AddSkeletonPartEvent& evt)
{
   LOG_DEBUG("AnimationStation:: Adding skeleton {path}", evt.filename);
   Maybe<BindingProperty> data = YAMLSerializer::DeserializeFile(evt.filename);
   if (!data)
   {
      data.Failure().WithContext("Failed loading {path}", evt.filename).Log();
      return;
   }

   Engine::Entity part = mEntities.Create(0, 0, 0);
   part.Get<Transform>()->SetParent(mPlayer);
   auto skeleton = part.Add<Skeleton>(*data);
   part.Add<VoxModel>(Asset::Model(skeleton->defaultModel));

   auto anims = part.Add<SkeletonAnimations>(skeleton->name);

   auto controller = mPlayer.Get<SimpleAnimationController>();
   controller->AddSkeleton(skeleton);
   controller->AddAnimations(anims);

   mPlayerParts.push_back(part);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

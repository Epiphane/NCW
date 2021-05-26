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
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "../Systems/EditorBackdropSystem.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ShaderLand
{

MainState::MainState(Engine::Input* input, Bounded& parent)
    : mMesh(&mEntities, Engine::Entity::ID(0, 0))
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
    // Create systems and configure
    DebugHelper::Instance().SetSystemManager(&mSystems);
    mSystems.Add<CameraSystem>(mInput);
    mSystems.Add<MakeshiftSystem>();
    mSystems.Add<EditorBackdropSystem>(&mCamera);
    mSystems.Add<Simple3DRenderSystem>(&mCamera);
    mSystems.Add<VoxelRenderSystem>(&mCamera);
    mSystems.Configure();

    // Unlock the mouse
    mInput->SetMouseLock(false);

    // Create entity that can resize the aspect ratio
    mEntities.Create().Add<Makeshift>([this]() {
        mPlayerCam->aspect = float(mParent.GetWidth()) / float(mParent.GetHeight());
        });

    // Create a mesh entity with ShadedMesh component
    mMesh = mEntities.Create(0, 0, 0);
    auto mesh = mMesh.Add<ShadedMesh>();

    auto cubes = mEntities.Create(0, 0, 0);
    std::vector<Voxel::Data> voxels;
    voxels.push_back(Voxel::Data());
    cubes.Add<VoxelRender>(std::move(voxels));

    // Create a camera
    Engine::Entity playerCamera = mEntities.Create(0, 0, 0);
    playerCamera.Get<Engine::Transform>()->SetLocalDirection(glm::vec3(-1, 0.5, -1));
    ArmCamera::Options cameraOptions;
    cameraOptions.aspect = float(mParent.GetWidth()) / float(mParent.GetHeight());
    cameraOptions.far = 1500.0f;
    cameraOptions.distance = 3.5f;
    cameraOptions.minDistance = 1;
    mPlayerCam = playerCamera.Add<ArmCamera>(playerCamera.Get<Engine::Transform>(), cameraOptions);
    playerCamera.Add<MouseDragCamera>(GLFW_MOUSE_BUTTON_LEFT);
    playerCamera.Add<MouseControlledCameraArm>();
    mCamera.Set(mPlayerCam.get());

    // Add floor and wireframe
    Engine::Entity entity = mEntities.Create(0, 0, 0);
    entity.Add<EditorWireframe>()->showNegative = false;
}

}; // namespace ShaderLand

}; // namespace Editor

}; // namespace CubeWorld

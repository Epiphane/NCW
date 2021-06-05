// By Thomas Steinke

#include <cassert>
#include <functional>
#include <imgui.h>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <Engine/Entity/Transform.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/Components/VoxModel.h>
#include <Shared/Helpers/Noise.h>
#include <Shared/Systems/AnimationSystem.h>
#include <Shared/Systems/AnimationEventSystem.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/CombatSystem.h>
#include <Shared/Systems/FollowerSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/BulletPhysicsDebug.h>
#include <Shared/Systems/BulletPhysicsSystem.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/SimpleParticleSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>
#include <Shared/Systems/WalkSystem.h>
#include <Shared/Systems/WalkAnimationSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "DynamicState.h"
#include "../Systems/ChunkManagementSystem.h"

namespace CubeWorld::Game
{

enum class SerializedComponent
{
    WalkSpeed,
    ChunkSpawnSource,
    BulletControlledBody,
    AnimationController,
    WalkDirector,
    ArmCamera,
    MouseControlledCamera,
    MouseControlledCameraArm,
    Follower,
    VoxModel,
    Unknown,
};

}; // namespace CubeWorld::Game

namespace meta
{

using CubeWorld::Game::SerializedComponent;

template<>
inline auto registerValues<SerializedComponent>()
{
    return values(
        value("walk_speed", SerializedComponent::WalkSpeed),
        value("chunk_spawn_source", SerializedComponent::ChunkSpawnSource),
        value("bullet_controlled_body", SerializedComponent::BulletControlledBody),
        value("animation_controller", SerializedComponent::AnimationController),
        value("walk_director", SerializedComponent::WalkDirector),
        value("arm_camera", SerializedComponent::ArmCamera),
        value("mouse_controlled_camera", SerializedComponent::MouseControlledCamera),
        value("mouse_controlled_camera_arm", SerializedComponent::MouseControlledCameraArm),
        value("follower", SerializedComponent::Follower),
        value("vox_model", SerializedComponent::VoxModel)
    );
}

}; // namespace meta

namespace CubeWorld::Game
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

DynamicState::DynamicState(Engine::Window& window)
    : mWorld(mEntities, mEvents)
    , mWindow(window)
{
    mSystems.Add<CameraSystem>(&window);
    mSystems.Add<AnimationSystem>();
    mSystems.Add<FlySystem>(&window);
    mSystems.Add<WalkSystem>(&window);
    mSystems.Add<WalkAnimationSystem>();
    mSystems.Add<AnimationApplicator>();
    mSystems.Add<FollowerSystem>();
    mSystems.Add<MakeshiftSystem>();
    auto physics = mSystems.Add<BulletPhysics::System>();
    auto debug = mSystems.Add<BulletPhysics::Debug>(physics, &mCamera);
    mSystems.Add<AnimationEventSystem>(physics);
    mSystems.Add<CombatSystem>();
    mSystems.Add<Simple3DRenderSystem>(&mCamera);
    mSystems.Add<VoxelRenderSystem>(&mCamera);
    mSystems.Add<SimpleParticleSystem>(&mCamera);
    mSystems.Add<ChunkManagementSystem>(&mWorld);

    // By default, no physics debugging.
    debug->SetActive(false);

    mSystems.Configure();
}

std::pair<uint64_t, Entity> DynamicState::InitEntity(const BindingProperty& data)
{
    Entity object = mEntities.Create();
    object.Add<Transform>(data["transform"]);

    return std::make_pair(data["id"].GetUint64Value(), object);
}

void DynamicState::InitComponents(
    const std::unordered_map<uint64_t, Entity>& entities,
    const BindingProperty& data
)
{
    Entity object = entities.at(data["id"].GetUint64Value());

    if (data["transform"]["parent"].IsUint64())
    {
        uint64_t parentId = data["transform"]["parent"].GetUint64Value();
        object.Get<Transform>()->SetParent(entities.at(parentId));
    }

    for (const auto& [component, props] : data["components"].pairs())
    {
        SerializedComponent cType = SerializedComponent::Unknown;
        Binding::deserialize(cType, component);

        Engine::ComponentHandle<ArmCamera> handle;
        switch (cType)
        {
        case SerializedComponent::WalkSpeed:
            object.Add<WalkSpeed>(props);
            break;
        case SerializedComponent::ChunkSpawnSource:
            object.Add<ChunkSpawnSource>(props);
            break;
        case SerializedComponent::BulletControlledBody:
            object.Add<BulletPhysics::ControlledBody>(props);
            break;
        case SerializedComponent::AnimationController:
            object.Add<AnimationController>(object.Get<Transform>(), mEntities, props);
            break;
        case SerializedComponent::WalkDirector:
            object.Add<WalkDirector>(entities, props);
            break;
        case SerializedComponent::ArmCamera:
            handle = object.Add<ArmCamera>(object.Get<Transform>(), props, float(mWindow.GetWidth()) / mWindow.GetHeight());
            mCamera.Set(handle.get());
            break;
        case SerializedComponent::MouseControlledCamera:
            object.Add<MouseControlledCamera>(props);
            break;
        case SerializedComponent::MouseControlledCameraArm:
            object.Add<MouseControlledCameraArm>(props);
            break;
        case SerializedComponent::Follower:
            object.Add<Follower>(entities, props);
            break;
        case SerializedComponent::VoxModel:
            object.Add<VoxModel>(props);
            break;
        default:
            assert(false);
            break;
        }
    }
}

void DynamicState::Load()
{
    Maybe<BindingProperty> maybeScene = YAMLSerializer::DeserializeFile(Asset::Path("Scenes/orb1.yaml"));
    if (!maybeScene)
    {
        maybeScene.Failure().Log();
        assert(false);
    }

    BindingProperty scene = std::move(*maybeScene);

    // Go over the list once, creating all the entities with no components.
    for (const BindingProperty& data : scene["objects"])
    {
        mDynamicEntities.emplace(InitEntity(data));
    }

    // Go over the list again, adding all the components (so they can resolve any inter-entity dependencies)
    for (const BindingProperty& data : scene["objects"])
    {
        InitComponents(mDynamicEntities, data);
    }
}

void DynamicState::Initialize()
{
    mWindow.SetMouseLock(true);

    Load();

    mDebugCallback = mWindow.AddCallback(Engine::Window::CtrlKey(GLFW_KEY_R), [&](int, int, int) {
        for (auto& [_, entity] : mDynamicEntities)
        {
            mEntities.Destroy(entity.GetID());
        }

        mDynamicEntities.clear();
        Load();
    });
}

void DynamicState::Pause()
{
    DebugHelper::Instance().SetSystemManager(nullptr);
}

void DynamicState::Unpause()
{
    DebugHelper::Instance().SetSystemManager(&mSystems);
}

void DynamicState::Update(TIMEDELTA dt)
{
    if (ImGui::Begin("Systems"))
    {
        mSystems.ForAll([&](const std::string& name, Engine::BaseSystem& system) {
            bool active = system.IsActive();
            if (ImGui::Checkbox(name.c_str(), &active))
            {
                system.SetActive(active);
            }
        });
    }
    ImGui::End();

    State::Update(dt);
}

}; // namespace CubeWorld::Game

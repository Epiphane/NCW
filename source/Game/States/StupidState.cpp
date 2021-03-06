// By Thomas Steinke

#include <cassert>
#include <functional>

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
#include "StupidState.h"
#include "../Systems/ChunkManagementSystem.h"

namespace CubeWorld
{

namespace Game
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

StupidState::StupidState(Engine::Window& window)
    : mWorld(mEntities, mEvents)
    , mWindow(window)
{
    DebugHelper::Instance().SetSystemManager(&mSystems);
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

    debug->SetActive(false);
    mDebugCallback = window.AddCallback(GLFW_KEY_L, [debug](int, int, int) {
        debug->SetActive(!debug->IsActive());
    });

    window.AddCallback(Engine::Window::CtrlKey(GLFW_KEY_R), [&](int, int, int) {
        mWorld.Reset();
    }).release();

    mSystems.Configure();
}

StupidState::~StupidState()
{
    DebugHelper::Instance().SetSystemManager(nullptr);
}

bool StupidState::BuildFloorCollision(int32_t size)
{
    static std::vector<bool> used(heights.size(), false);

    int blocksCreated = 0;

    auto index = [&](int i, int j) {
        return size_t(i * (2 * size_t(size) + 1) + j);
    };

    auto makeCollider = [&](int i, int j, int height, int width, int length) {
        Entity collider = mEntities.Create(i - size + float(width - 1) / 2, float(height), j - size + float(length - 1) / 2);
        collider.Add<BulletPhysics::StaticBody>(glm::vec3(width, 1, length));

        for (int x = i; x < i + width; ++x)
        {
            for (int y = j; y < j + length; ++y)
            {
                assert(!used[index(x, y)]);
                if (heights[index(x, y)] == height) used[index(x, y)] = true;
            }
        }
    };

    for (int i = 0; i < 2 * size + 1; ++i) {
        for (int j = 0; j < 2 * size + 1; ++j) {
            size_t ndx = index(i, j);
            if (used[ndx])
            {
                continue;
            }

            int32_t height = heights[ndx];

            // Attempt 3: Same as 2, but allow blocks to sit under each other
            // Result: Generated 933 blocks
            int width = 0, length = 0;
            int nWidth = 1, nLength = 1;
            while (nWidth > width || nLength > length)
            {
                width = nWidth++;
                length = nLength++;

                if (i + nWidth - 1 >= 2 * size + 1)
                {
                    --nWidth;
                }
                else
                {
                    for (int n = 0; n < length; ++n)
                    {
                        if (
                            used[index(i + nWidth - 1, j + n)] ||
                            heights[index(i + nWidth - 1, j + n)] < height
                            )
                        {
                            --nWidth;
                            break;
                        }
                    }
                }

                if (j + nLength - 1 >= 2 * size + 1)
                {
                    --nLength;
                }
                else
                {
                    for (int n = 0; n < nWidth; ++n)
                    {
                        if (
                            used[index(i + n, j + nLength - 1)] ||
                            heights[index(i + n, j + nLength - 1)] < height
                            )
                        {
                            --nLength;
                            break;
                        }
                    }
                }
            }

            makeCollider(i, j, height, width, length);
            ++blocksCreated;
        }
    }

    return false;
}

void StupidState::Initialize()
{
    mWindow.SetMouseLock(true);

    // Create player first so it gets index 0.
    Entity player = mEntities.Create();

    if (0)
    {
        Entity dummy = mEntities.Create(5, 10, 10);
        dummy.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
        auto dummyController = dummy.Add<AnimationController>();

        dummy.Add<UnitComponent>(5);

        Engine::Entity part = mEntities.Create(0, 0, 0);
        part.Get<Transform>()->SetParent(dummy);
        part.Add<VoxModel>(Asset::Model("character.vox"))->mTint = glm::vec3(0, 168.0f, 0);
        dummyController->AddSkeleton(part.Add<Skeleton>(Asset::Skeleton("character.yaml")));
        dummyController->AddAnimations(part.Add<SkeletonAnimations>("character"));

        std::unique_ptr<btCapsuleShape> playerShape = std::make_unique<btCapsuleShape>(0.75f, 1.25f);
        std::unique_ptr<btPairCachingGhostObject> ghostObject = std::make_unique<btPairCachingGhostObject>();
        ghostObject->setWorldTransform(btTransform(btQuaternion(1, 0, 0, 1), btVector3(5, 10, 10)));
        ghostObject->setCollisionShape(playerShape.get());
        ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
        std::unique_ptr<btKinematicCharacterController> controller = std::make_unique<btKinematicCharacterController>(ghostObject.get(), playerShape.get(), 1.5f);
        controller->setUp(btVector3{ 0, 1, 0 });

        dummy.Add<BulletPhysics::ControlledBody>(std::move(playerShape), std::move(ghostObject), std::move(controller));
    }

    player.Add<UnitComponent>(5);
    player.Add<Transform>(glm::vec3(-48, 40, 20), glm::vec3(0, 0, 1));
    player.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
    player.Get<Transform>()->SetLocalPosition({ 3, 0, 0 });
    player.Add<WalkSpeed>(0.20f, 0.04f, 0.45f);
    player.Add<ChunkSpawnSource>();

    // Set up the player controller
    {
        std::unique_ptr<btCapsuleShape> playerShape = std::make_unique<btCapsuleShape>(0.75f, 0.75f);
        std::unique_ptr<btPairCachingGhostObject> ghostObject = std::make_unique<btPairCachingGhostObject>();
        ghostObject->setWorldTransform(btTransform(btQuaternion(1, 0, 0, 1), btVector3(0, 10, 0)));
        ghostObject->setCollisionShape(playerShape.get());
        ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
        std::unique_ptr<btKinematicCharacterController> controller = std::make_unique<btKinematicCharacterController>(ghostObject.get(), playerShape.get(), 1.5f, btVector3{ 0, 1, 0 });

        player.Add<BulletPhysics::ControlledBody>(std::move(playerShape), std::move(ghostObject), std::move(controller));
    }

    auto controller = player.Add<AnimationController>();

    /*
    player.Add<Makeshift>([this, player](Engine::EntityManager&, Engine::EventManager&, TIMEDELTA dt) {
       auto anim = player.Get<AnimationController>();
       if (mWindow.IsMouseDown(GLFW_MOUSE_BUTTON_LEFT))
       {
          anim->SetParameter("fighting", 5.0f);
       }

       anim->SetBoolParameter("attack", mWindow.IsMouseDown(GLFW_MOUSE_BUTTON_LEFT));
       anim->SetParameter("fighting", anim->GetFloatParameter("fighting") - float(dt));
    });
    */

    Engine::Entity part = mEntities.Create(0, 0, 0);
    part.Get<Transform>()->SetParent(player);
    part.Add<VoxModel>(Asset::Model("character.vox"))->mTint = glm::vec3(0, 0, 168.0f);
    controller->AddSkeleton(part.Add<Skeleton>(Asset::Skeleton("character.yaml")));
    controller->AddAnimations(part.Add<SkeletonAnimations>("character"));

    constexpr int HAMMER = 1;
    if (HAMMER)
    {
        part = mEntities.Create(0, 0, 0);
        part.Get<Transform>()->SetParent(player);
        part.Add<VoxModel>(Asset::Model("wood-greatmace02.vox"));
        controller->AddSkeleton(part.Add<Skeleton>(Asset::Skeleton("greatmace.yaml")));
        controller->AddAnimations(part.Add<SkeletonAnimations>("greatmace"));
    }
    else
    {
        part = mEntities.Create(0, 0, 0);
        part.Get<Transform>()->SetParent(player);
        part.Add<VoxModel>(Asset::Model("iron-sword1-random2.vox"));
        controller->AddSkeleton(part.Add<Skeleton>(Asset::Skeleton("sword_right.yaml")));
        controller->AddAnimations(part.Add<SkeletonAnimations>("sword_right"));

        part = mEntities.Create(0, 0, 0);
        part.Get<Transform>()->SetParent(player);
        part.Add<VoxModel>(Asset::Model("iron-sword1-random1.vox"));
        controller->AddSkeleton(part.Add<Skeleton>(Asset::Skeleton("sword_left.yaml")));
        controller->AddAnimations(part.Add<SkeletonAnimations>("sword_left"));
    }

    Entity playerCamera = mEntities.Create(0, 0, 0);
    ArmCamera::Options cameraOptions;
    cameraOptions.aspect = float(mWindow.GetWidth()) / mWindow.GetHeight();
    cameraOptions.far = 1500.0f;
    cameraOptions.distance = 3.5f;
    Engine::ComponentHandle<ArmCamera> handle = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
    playerCamera.Add<MouseControlledCamera>();
    playerCamera.Add<MouseControlledCameraArm>();
    playerCamera.Add<Follower>(player.Get<Transform>(), glm::vec3{ 10.0f, 5.0f, 10.0f });
    player.Add<WalkDirector>(playerCamera.Get<Transform>(), false);

    // Create a campfire
    Engine::Entity campfire = mEntities.Create(-1, -16, 0);
    campfire.Add<VoxModel>(Asset::Model("house1.vox"));

    mCamera.Set(handle.get());
}

}; // namespace Game

}; // namespace CubeWorld

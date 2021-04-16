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
#include "MainState.h"

namespace CubeWorld
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

std::unique_ptr<Engine::Input::KeyCallbackLink> gConfigCallback;
MainState::MainState(Engine::Window& window) : mWindow(window)
{
    DebugHelper::Instance().SetSystemManager(&mSystems);
    mSystems.Add<CameraSystem>(&window);
    mSystems.Add<AnimationSystem>();
    mSystems.Add<FlySystem>(&window);
    mSystems.Add<WalkSystem>(&window);
    mSystems.Add<WalkAnimationSystem>();
    mSystems.Add<AnimationApplicator>();
    mSystems.Add<BulletPhysics::System>();
    mSystems.Add<BulletPhysics::Debug>(mSystems.Get<BulletPhysics::System>(), &mCamera);
    mSystems.Add<AnimationEventSystem>(mSystems.Get<BulletPhysics::System>());
    mSystems.Add<FollowerSystem>();
    mSystems.Add<MakeshiftSystem>();
    mSystems.Add<CombatSystem>();
    mSystems.Add<Simple3DRenderSystem>(&mCamera);
    mSystems.Add<VoxelRenderSystem>(&mCamera);
    mSystems.Add<SimpleParticleSystem>(&mCamera);

    gConfigCallback = window.AddCallback(GLFW_KEY_Y, [this](int, int, int) {
        mSystems.Get<VoxelRenderSystem>()->Reconfigure();
    });

    BulletPhysics::Debug* debug = mSystems.Get<BulletPhysics::Debug>();
    debug->SetActive(false);
    mDebugCallback = window.AddCallback(GLFW_KEY_L, [debug](int, int, int) {
        debug->SetActive(!debug->IsActive());
    });

    mSystems.Configure();
}

MainState::~MainState()
{
    DebugHelper::Instance().SetSystemManager(nullptr);
}

float angle_ = 0;
float roll_ = 0;
void MainState::Initialize()
{
    mWindow.SetMouseLock(true);

    // Create player first so it gets index 0.
    Entity player = mEntities.Create();
    player.Add<Transform>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    player.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
    player.Add<FlySpeed>(4*25.0f);

    // Set up the player controller
    {
        player.Add<BulletPhysics::DynamicBody>(glm::vec3(1.0f), 1.f);
        player.Get<BulletPhysics::DynamicBody>()->body->setGravity(btVector3{0, 0, 0});
    }

    Engine::Entity part = mEntities.Create(0, 0, 0);
    part.Get<Transform>()->SetParent(player);
    part.Get<Transform>()->SetLocalPosition(glm::vec3{ 0, 2.0f, 0 });
    part.Get<Transform>()->SetLocalScale(glm::vec3{0.25f});
    part.Add<VoxModel>(Asset::Model("bird.vox"))->mTint = glm::vec3(0, 0, 168.0f);

    part.Add<Makeshift>([part, this](Engine::EntityManager&, Engine::EventManager&, TIMEDELTA) {
        int isW = mWindow.IsKeyDown(GLFW_KEY_W) ? 1 : 0;
        int isA = mWindow.IsKeyDown(GLFW_KEY_A) ? 1 : 0;
        int isS = mWindow.IsKeyDown(GLFW_KEY_S) ? 1 : 0;
        int isD = mWindow.IsKeyDown(GLFW_KEY_D) ? 1 : 0;

        float roll = 0;
        float angle = 0;
        float sign = 0;
        if (isA)
        {
            angle += MATH_PI / 2.0f;
            roll -= MATH_PI / 3.0f;
            ++sign;
        }
        if (isD)
        {
            angle -= MATH_PI / 2.0f;
            roll += MATH_PI / 3.0f;
            --sign;
        }
        if (isW)
        {
            angle -= (float)(sign * MATH_PI / 4.0f);
            roll /= 2.0f;
        }
        if (isS)
        {
            angle += (float)(sign * MATH_PI / 4.0f);
            roll = float(MATH_PI - roll);
        }
        if (isS && !isW)
        {
            if (!isA && !isD)
            {
                angle += MATH_PI;
            }
            roll /= 2.0f;
        }

        if (angle_ - angle > MATH_PI)
        {
            angle_ -= 2 * MATH_PI;
        }
        if (angle - angle_ > MATH_PI)
        {
            angle_ += 2 * MATH_PI;
        }

        if ((isW + isA + isS + isD) != 0)
        {
            angle_ = (angle_ + angle) / 2.0f;
        }
        roll_ = (roll_ + roll) / 2.0f;//0.4f * angle_ + 0.6f * angle;

        if (angle_ < MATH_PI)
        {
            angle += 2 * MATH_PI;
        }
        if (angle_ > MATH_PI)
        {
            angle -= 2 * MATH_PI;
        }

        part.Get<Transform>()->SetYaw(angle_);
        part.Get<Transform>()->SetRoll(roll_);
    });

    Entity playerCamera = mEntities.Create(0, 4, 0);
    ArmCamera::Options cameraOptions;
    cameraOptions.aspect = float(mWindow.GetWidth()) / mWindow.GetHeight();
    cameraOptions.far = 1500.0f;
    cameraOptions.distance = 3.5f;
    Engine::ComponentHandle<ArmCamera> handle = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
    playerCamera.Add<MouseControlledCamera>();
    playerCamera.Add<MouseControlledCameraArm>();
    playerCamera.Add<Follower>(player.Get<Transform>(), glm::vec3{0.0f});
    player.Get<FlySpeed>()->director = playerCamera.Get<Transform>();

    // Create a campfire
    mCamera.Set(handle.get());

    mWorld.Build();

    int kSize = 20;
    for (int dist = 0; dist < kSize; dist++)
    {
        mWorld.Create(dist, 0, 0, mEntities);
        mWorld.Create(0, 0, dist, mEntities);
        mWorld.Create(-dist, 0, 0, mEntities);
        mWorld.Create(0, 0, -dist, mEntities);
        for (int d = 1; d <= dist; d++)
        {
            mWorld.Create(dist, 0, d, mEntities);
            mWorld.Create(dist, 0, -d, mEntities);
            mWorld.Create(d, 0, dist, mEntities);
            mWorld.Create(-d, 0, dist, mEntities);
            mWorld.Create(-dist, 0, d, mEntities);
            mWorld.Create(-dist, 0, -d, mEntities);
            mWorld.Create(d, 0, -dist, mEntities);
            mWorld.Create(-d, 0, -dist, mEntities);
        }
    }

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(kPrimitiveRestart);
}

}; // namespace CubeWorld

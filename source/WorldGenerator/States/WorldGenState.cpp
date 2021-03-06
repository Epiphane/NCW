// By Thomas Steinke

#include <cassert>
#include <functional>
#include <imgui.h>

#include <RGBDesignPatterns/Macros.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <Engine/Entity/Transform.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/Components/Data.h>
#include <Shared/Components/VoxModel.h>
#include <Shared/Systems/AnimationSystem.h>
#include <Shared/Systems/AnimationEventSystem.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/CombatSystem.h>
#include <Shared/Systems/FollowerSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/JavascriptSystem.h>
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
#include "WorldGenState.h"

namespace CubeWorld
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

WorldGenState::WorldGenState(Engine::Input* input, Bounded& parent)
    : mWorld(mEntities, mEvents)
    , mInput(input)
    , mParent(parent)
{
    mEvents.Subscribe<JavascriptEvent>(*this);
}

WorldGenState::~WorldGenState()
{
    DebugHelper::Instance().SetSystemManager(nullptr);
}

void WorldGenState::Initialize()
{
    DebugHelper::Instance().SetSystemManager(&mSystems);
    mSystems.Add<AnimationSystem>();
    mSystems.Add<CameraSystem>(mInput);
    mSystems.Add<JavascriptSystem>(mInput);
    mSystems.Add<FlySystem>(mInput);
    mSystems.Add<WalkSystem>(mInput);
    mSystems.Add<WalkAnimationSystem>();
    mSystems.Add<AnimationApplicator>();
    auto physics = mSystems.Add<BulletPhysics::System>();
    mSystems.Add<BulletPhysics::Debug>(physics, &mCamera)->SetActive(false);
    mSystems.Add<AnimationEventSystem>(physics);
    mSystems.Add<FollowerSystem>();
    mSystems.Add<MakeshiftSystem>();
    mSystems.Add<CombatSystem>();
    mSystems.Add<Simple3DRenderSystem>(&mCamera);
    mSystems.Add<SimpleParticleSystem>(&mCamera);
    mSystems.Configure();

    mInput->SetMouseLock(false);

    // Create player first so it gets index 0.
    Entity player = mEntities.Create();
    player.Add<Transform>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    player.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));

    // Set up the player controller
    {
        player.Add<BulletPhysics::DynamicBody>(glm::vec3(1.0f), 1.f);
        player.Get<BulletPhysics::DynamicBody>()->body->setGravity(btVector3{0, 0, 0});
    }

    Engine::Entity part = mEntities.Create(0, 0, 0);
    part.Get<Transform>()->SetParent(player);
    part.Get<Transform>()->SetLocalPosition(glm::vec3{ 0, 2.0f, 0 });
    part.Get<Transform>()->SetLocalScale(glm::vec3{0.25f});
    //part.Add<VoxModel>(Asset::Model("bird.vox"))->mTint = glm::vec3(0, 0, 168.0f);

    part.Add<Data>();
    part.Add<Makeshift>([part, this](Engine::EntityManager&, Engine::EventManager&, TIMEDELTA) {
        Data& data = *part.Get<Data>();

        int isW = mInput->IsKeyDown(GLFW_KEY_W) ? 1 : 0;
        int isA = mInput->IsKeyDown(GLFW_KEY_A) ? 1 : 0;
        int isS = mInput->IsKeyDown(GLFW_KEY_S) ? 1 : 0;
        int isD = mInput->IsKeyDown(GLFW_KEY_D) ? 1 : 0;

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

        float& prevAngle = data.Float("angle");
        if (double(prevAngle) - angle > MATH_PI)
        {
            prevAngle -= 2 * MATH_PI;
        }
        if (double(angle) - prevAngle > MATH_PI)
        {
            prevAngle += 2 * MATH_PI;
        }

        if ((isW + isA + isS + isD) != 0)
        {
            prevAngle = (prevAngle + angle) / 2.0f;
        }
        float& prevRoll = data.Float("roll");
        prevRoll = (prevRoll + roll) / 2.0f;

        if (prevAngle < MATH_PI)
        {
            angle += 2 * MATH_PI;
        }
        if (prevAngle > MATH_PI)
        {
            angle -= 2 * MATH_PI;
        }

        part.Get<Transform>()->SetYaw(prevAngle);
        part.Get<Transform>()->SetRoll(prevRoll);
    });

    Entity playerCamera = mEntities.Create(0, 2, 0);
    ArmCamera::Options cameraOptions;
    cameraOptions.aspect = float(mParent.GetWidth()) / mParent.GetHeight();
    cameraOptions.far = 1500.0f;
    cameraOptions.distance = 3.5f;
    cameraOptions.maxDistance = 1000;
    Engine::ComponentHandle<ArmCamera> handle = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
    playerCamera.Add<MouseDragCamera>(GLFW_MOUSE_BUTTON_LEFT, -0.007, 0.007);
    playerCamera.Add<MouseControlledCamera>();
    playerCamera.Add<MouseControlledCameraArm>();
    playerCamera.Add<Follower>(player.Get<Transform>(), glm::vec3{0.0f});
    playerCamera.Add<Makeshift>([&] {
       ArmCamera* cam = static_cast<ArmCamera*>(mCamera.Get());
       cam->aspect = float(mParent.GetWidth()) / float(mParent.GetHeight());
    });

    mCamera.Set(handle.get());

    mWorld.Build();
    Receive(JavascriptEvent("rebuild_world"));

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(kPrimitiveRestart);
}

void WorldGenState::Receive(const JavascriptEvent& evt)
{
    if (evt.name == "rebuild_world")
    {
        mWorld.Reset();

        mWorld.EnsureLoaded(0, 0, 0);
        int kSize = 4;
        for (int dist = 1; dist < kSize; dist++)
        {
            mWorld.EnsureLoaded(dist, 0, 0);
            mWorld.EnsureLoaded(0, 0, dist);
            mWorld.EnsureLoaded(-dist, 0, 0);
            mWorld.EnsureLoaded(0, 0, -dist);
            for (int d = 1; d <= dist; d++)
            {
                mWorld.EnsureLoaded(dist, 0, d);
                mWorld.EnsureLoaded(dist, 0, -d);
                mWorld.EnsureLoaded(d, 0, dist);
                mWorld.EnsureLoaded(-d, 0, dist);
                mWorld.EnsureLoaded(-dist, 0, d);
                mWorld.EnsureLoaded(-dist, 0, -d);
                mWorld.EnsureLoaded(d, 0, -dist);
                mWorld.EnsureLoaded(-d, 0, -dist);
            }
        }
    }
}

}; // namespace CubeWorld

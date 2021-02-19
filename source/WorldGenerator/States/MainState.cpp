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

bool MainState::BuildFloorCollision(int32_t size)
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

float angle_ = 0;
float roll_ = 0;
void MainState::Initialize()
{
    mWindow.SetMouseLock(true);

    // Create player first so it gets index 0.
    Entity player = mEntities.Create();
    player.Add<Transform>(glm::vec3(0, 2, 0), glm::vec3(0, 0, 1));
    player.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
    player.Add<FlySpeed>(25.0f);

    // Set up the player controller
    {
        player.Add<BulletPhysics::DynamicBody>(glm::vec3(1.0f), 1.f);
        player.Get<BulletPhysics::DynamicBody>()->body->setGravity(btVector3{0, 0, 0});
    }

    Engine::Entity part = mEntities.Create(0, 0, 0);
    part.Get<Transform>()->SetParent(player);
    part.Get<Transform>()->SetLocalPosition(glm::vec3{0, 10.0f, 0});
    part.Add<VoxModel>(Asset::Model("bird.vox"))->mTint = glm::vec3(0, 0, 168.0f);

    part.Add<Makeshift>([part, player, this](Engine::EntityManager&, Engine::EventManager&, TIMEDELTA) {
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

        DebugHelper::Instance().SetMetric("angle", angle);
        DebugHelper::Instance().SetMetric("angle_", angle_);
        part.Get<Transform>()->SetYaw(angle_);
        part.Get<Transform>()->SetRoll(roll_);
    });

    Entity playerCamera = mEntities.Create(0, 30, 0);
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
    for (int i = -5; i < 5; i++)
    {
        for (int j = -5; j < 5; j++)
        {
            mWorld.Create(i, 0, j, mEntities);
        }
    }

    //chunk.Get<Transform>()->SetLocalPosition(glm::vec3{0});

    if (0)
    {
        // Add some voxels.
        std::vector<Voxel::Data> carpet;
        std::vector<glm::vec3> points;
        std::vector<glm::vec3> colors;

        noise::module::Perlin heightmodule;
        noise::utils::NoiseMap heightmap;
        noise::utils::NoiseMapBuilderPlane builder;
        heightmodule.SetFrequency(0.5);
        builder.SetSourceModule(heightmodule);
        builder.SetDestNoiseMap(heightmap);
        const int size = 50;
        builder.SetDestSize(2 * size + 1, 2 * size + 1);
        builder.SetBounds(6, 10, 1, 5);
        builder.Build();

        // Colors
        /*
        -1.0000, (  0,   0, 128, 255)); // deeps
        -0.2500, (  0,   0, 255, 255)); // shallow
        0.0000, (  0, 128, 255, 255)); // shore
        0.0625, (240, 240,  64, 255)); // sand
        0.1250, ( 32, 160,   0, 255)); // grass
        0.3750, (224, 224,   0, 255)); // dirt
        0.7500, (128, 128, 128, 255)); // rock
        1.0000, (255, 255, 255, 255)); // snow
        */
        glm::vec4 DEEP(0, 0, 128, 1);
        glm::vec4 SHALLOW(0, 0, 255, 1);
        glm::vec4 SHORE(0, 128, 255, 1);
        glm::vec4 SAND(240, 240, 64, 1);
        glm::vec4 GRASS(32, 160, 0, 1);
        glm::vec4 DIRT(224, 224, 0, 1);
        glm::vec4 ROCK(128, 128, 128, 1);
        glm::vec4 SNOW(255, 255, 255, 1);

        heights.clear();
        heights.resize(4 * (size + 1) * (size + 1));

        for (int i = -size; i <= size; ++i) {
            int rowIndex = (i + size) * (2 * size + 1);
            for (int j = -size; j <= size; ++j) {
                float elevation = 0.25f + 2 * float(pow(heightmap.GetValue(i + size, j + size), 2));
                glm::vec4 source, dest;
                float start, end;
                if (elevation >= 0.75f) { source = ROCK; dest = SNOW; start = 0.75f; end = 1.0f; }
                else if (elevation >= 0.375f) { source = DIRT; dest = ROCK; start = 0.375f; end = 0.75f; }
                else if (elevation >= 0.125f) { source = GRASS; dest = DIRT; start = 0.125f; end = 0.375f; }
                else if (elevation >= 0.0625f) { source = SAND; dest = GRASS; start = 0.0625f; end = 0.125f; }
                else if (elevation >= 0.0f) { source = SHORE; dest = SAND; start = 0; end = 0.0625f; }
                else if (elevation >= -0.25f) { source = SHALLOW; dest = SHORE; start = -0.25f; end = 0; }
                else { source = DEEP; dest = SHALLOW; start = -1.0f; end = -0.25f; }
                float perc = (elevation - start) / (end - start);

                glm::vec3 position = glm::vec3(i, std::round(elevation * 8) - 4, j);
                glm::vec4 color = dest * perc + source * (1 - perc);
                carpet.push_back(Voxel::Data(position, color, Voxel::All));

                heights[uint64_t(rowIndex) + j + size] = int32_t(position.y);
            }
        }

        assert(carpet.size() > 0);

        Entity voxels = mEntities.Create(0, 0, 0);
        voxels.Add<VoxelRender>(std::move(carpet));

        BuildFloorCollision(size);
    }
}

}; // namespace CubeWorld

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
#include "RenderTestState.h"

namespace CubeWorld
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

RenderTestState::RenderTestState(Engine::Window& window) : mWindow(window)
{
    DebugHelper::Instance().SetSystemManager(&mSystems);
    mSystems.Add<CameraSystem>(&window);
    mSystems.Add<AnimationSystem>();
    mSystems.Add<FlySystem>(&window);
    mSystems.Add<WalkSystem>(&window);
    mSystems.Add<FollowerSystem>();
    mSystems.Add<MakeshiftSystem>();
    mSystems.Add<CombatSystem>();
    mSystems.Add<Simple3DRenderSystem>(&mCamera);
    mSystems.Add<VoxelRenderSystem>(&mCamera);
    mSystems.Add<SimpleParticleSystem>(&mCamera);

    mSystems.Configure();
}

RenderTestState::~RenderTestState()
{
    DebugHelper::Instance().SetSystemManager(nullptr);
}

void RenderTestState::Initialize()
{
    mWindow.SetMouseLock(true);

    Entity camera = mEntities.Create(0, 0, 0);
    ArmCamera::Options cameraOptions;
    cameraOptions.aspect = float(mWindow.GetWidth()) / mWindow.GetHeight();
    cameraOptions.far = 1500.0f;
    cameraOptions.distance = 3.5f;
    Engine::ComponentHandle<ArmCamera> handle = camera.Add<ArmCamera>(camera.Get<Transform>(), cameraOptions);
    camera.Add<MouseControlledCamera>();
    camera.Add<MouseControlledCameraArm>();
    mCamera.Set(handle.get());

    Entity axes = mEntities.Create(0, 0, 0);
    std::vector<Voxel::Data> voxels;
    voxels.push_back(Voxel::Data(glm::vec3(10, 0, 0), glm::vec4(255, 0, 0, 1), Voxel::All, 0x12345678));
    voxels.push_back(Voxel::Data(glm::vec3(0, 10, 0), glm::vec4(0, 255, 0, 1), Voxel::All, 0x12345678));
    voxels.push_back(Voxel::Data(glm::vec3(0, 0, 10), glm::vec4(0, 0, 255, 1), Voxel::All, 0x12345678));
    axes.Add<VoxelRender>(std::move(voxels));

    if (0)
    {
        Entity test1 = mEntities.Create(0, 0, 0);

        constexpr int kk = 100;
        for (int i = -kk; i < kk; i++)
        {
            for (int j = -kk; j < kk; j++)
            {
                for (int k = -kk; k < kk; k++)
                {
                    if (rand() % 3 == 1)
                    {
                        voxels.push_back(Voxel::Data(glm::vec3(i, j, k), glm::vec4(255, 255, 255, 1), Voxel::All, 0x13579bdf));
                    }
                }
            }
        }
        test1.Add<VoxelRender>(std::move(voxels));
    }

    if (0)
    {
        glm::vec3 origin{1, 1, 1};
        glm::vec3 o000 = 15.f / 16.f * origin;
        glm::vec3 o001 = 13.f / 16.f * origin;
        glm::vec3 o010 = 11.f / 16.f * origin;
        glm::vec3 o011 = 9.f / 16.f * origin;
        glm::vec3 o100 = 7.f / 16.f * origin;
        glm::vec3 o101 = 5.f / 16.f * origin;
        glm::vec3 o110 = 3.f / 16.f * origin;
        glm::vec3 o111 = 1.f / 16.f * origin;

        const float d = 0.5f;
        glm::vec3 p000{-d, -d, -d};
        glm::vec3 p001{-d, -d, d};
        glm::vec3 p010{-d, d, -d};
        glm::vec3 p011{-d, d, d};
        glm::vec3 p100{d, -d, -d};
        glm::vec3 p101{d, -d, d};
        glm::vec3 p110{d, d, -d};
        glm::vec3 p111{d, d, d};

        Entity test2 = mEntities.Create(1.5f, 0, 0);
        std::vector<glm::vec3> points{
            p111,
            (p111 + p110) / 2.0f,
            (p111 + p010) / 2.0f,
            (p111 + p011) / 2.0f,
            (p111 + p001) / 2.0f,
            (p111 + p101) / 2.0f,
            (p111 + p100) / 2.0f,
            // 7
            p110,
            (p110 + p010) / 2.0f,
            (p110 + p000) / 2.0f,
            (p110 + p100) / 2.0f,
            // 11
            p010,
            (p010 + p000) / 2.0f,
            (p010 + p001) / 2.0f,
            (p010 + p011) / 2.0f,
            // 15
            p011,
            (p011 + p001) / 2.0f,
            // 17
            p101,
            (p101 + p100) / 2.0f,
            (p101 + p000) / 2.0f,
            (p101 + p001) / 2.0f,
            // 21
            p001,
            (p001 + p000) / 2.0f,
            // 23
            p000,
            (p000 + p100) / 2.0f,
            // 25
            p100,
        };
        std::vector<glm::vec3> colors{
            o111,
            (o111 + o110) / 2.0f,
            (o111 + o010) / 2.0f,
            (o111 + o011) / 2.0f,
            (o111 + o001) / 2.0f,
            (o111 + o101) / 2.0f,
            (o111 + o100) / 2.0f,

            o110,
            (o110 + o010) / 2.0f,
            (o110 + o000) / 2.0f,
            (o110 + o100) / 2.0f,

            o010,
            (o010 + o000) / 2.0f,
            (o010 + o001) / 2.0f,
            (o010 + o011) / 2.0f,

            o011,
            (o011 + o001) / 2.0f,

            o101,
            (o101 + o100) / 2.0f,
            (o101 + o000) / 2.0f,
            (o101 + o001) / 2.0f,

            o001,
            (o001 + o000) / 2.0f,

            o000,
            (o000 + o100) / 2.0f,

            o100,
        };


        GLuint PRIMITIVE_RESTART = 12345;
        std::vector<GLuint> indices{
            0, 1, 2, 3, 4, 5, 6, 1, PRIMITIVE_RESTART,
            7, 1, 6, 10, 9, 8, 2, 1, PRIMITIVE_RESTART,
            11, 2, 8, 9, 12, 13, 14, 2, PRIMITIVE_RESTART,
            15, 3, 2, 14, 13, 16, 4, 3, PRIMITIVE_RESTART,
            17, 18, 6, 5, 4, 20, 19, 18, PRIMITIVE_RESTART,
            21, 20, 4, 16, 13, 22, 19, 20, PRIMITIVE_RESTART,
            23, 19, 22, 13, 12, 9, 24, 19, PRIMITIVE_RESTART,
            25, 6, 18, 19, 24, 9, 10, 6, PRIMITIVE_RESTART
        };

        std::vector<glm::vec3> ps, cs;

        constexpr int kk = 100;
        for (int i = -kk; i < kk; i++)
        {
            for (int j = -kk; j < kk; j++)
            {
                for (int k = -kk; k < kk; k++)
                {
                    if (rand() % 3 == 1)
                    {
                        for (const auto& n : indices)
                        {
                            if (n != PRIMITIVE_RESTART)
                            {
                                ps.push_back(points[n] + glm::vec3(i, j, k));
                                cs.push_back(colors[n]);
                            }
                        }
                    }
                }
            }
        }

        test2.Add<Simple3DRender>(std::move(ps), std::move(cs));
        test2.Get<Simple3DRender>()->renderType = GL_TRIANGLE_FAN;
    }

    if (0)
    {
        glm::vec3 origin{1, 1, 1};
        glm::vec3 o000 = 15.f / 16.f * origin;
        glm::vec3 o001 = 13.f / 16.f * origin;
        glm::vec3 o010 = 11.f / 16.f * origin;
        glm::vec3 o011 = 9.f / 16.f * origin;
        glm::vec3 o100 = 7.f / 16.f * origin;
        glm::vec3 o101 = 5.f / 16.f * origin;
        glm::vec3 o110 = 3.f / 16.f * origin;
        glm::vec3 o111 = 1.f / 16.f * origin;

        const float d = 0.5f;
        glm::vec3 p000{-d, -d, -d};
        glm::vec3 p001{-d, -d, d};
        glm::vec3 p010{-d, d, -d};
        glm::vec3 p011{-d, d, d};
        glm::vec3 p100{d, -d, -d};
        glm::vec3 p101{d, -d, d};
        glm::vec3 p110{d, d, -d};
        glm::vec3 p111{d, d, d};

        Entity test = mEntities.Create(-1.5f, 0, 0);
        std::vector<glm::vec3> points{
            p111,
            (p111 + p110) / 2.0f,
            (p111 + p010) / 2.0f,
            (p111 + p011) / 2.0f,
            (p111 + p001) / 2.0f,
            (p111 + p101) / 2.0f,
            (p111 + p100) / 2.0f,

            // 7
            p110,
            (p110 + p010) / 2.0f,
            (p110 + p000) / 2.0f,
            (p110 + p100) / 2.0f,

            // 11
            p010,
            (p010 + p000) / 2.0f,
            (p010 + p001) / 2.0f,
            (p010 + p011) / 2.0f,

            // 15
            p011,
            (p011 + p001) / 2.0f,

            // 17
            p101,
            (p101 + p100) / 2.0f,
            (p101 + p000) / 2.0f,
            (p101 + p001) / 2.0f,

            // 21
            p001,
            (p001 + p000) / 2.0f,

            // 23
            p000,
            (p000 + p100) / 2.0f,

            // 25
            p100,
        };
        std::vector<glm::vec3> colors{
            o111,
            (o111 + o110) / 2.0f,
            (o111 + o010) / 2.0f,
            (o111 + o011) / 2.0f,
            (o111 + o001) / 2.0f,
            (o111 + o101) / 2.0f,
            (o111 + o100) / 2.0f,

            o110,
            (o110 + o010) / 2.0f,
            (o110 + o000) / 2.0f,
            (o110 + o100) / 2.0f,

            o010,
            (o010 + o000) / 2.0f,
            (o010 + o001) / 2.0f,
            (o010 + o011) / 2.0f,

            o011,
            (o011 + o001) / 2.0f,

            o101,
            (o101 + o100) / 2.0f,
            (o101 + o000) / 2.0f,
            (o101 + o001) / 2.0f,

            o001,
            (o001 + o000) / 2.0f,

            o000,
            (o000 + o100) / 2.0f,

            o100,
        };

        GLuint PRIMITIVE_RESTART = 12345;
        std::vector<GLuint> indices{
            0, 1, 2, 3, 4, 5, 6, 1, PRIMITIVE_RESTART,
            7, 1, 6, 10, 9, 8, 2, 1, PRIMITIVE_RESTART,
            11, 2, 8, 9, 12, 13, 14, 2, PRIMITIVE_RESTART,
            15, 3, 2, 14, 13, 16, 4, 3, PRIMITIVE_RESTART,
            17, 18, 6, 5, 4, 20, 19, 18, PRIMITIVE_RESTART,
            21, 20, 4, 16, 13, 22, 19, 20, PRIMITIVE_RESTART,
            23, 19, 22, 13, 12, 9, 24, 19, PRIMITIVE_RESTART,
            25, 6, 18, 19, 24, 9, 10, 6, PRIMITIVE_RESTART
        };
        std::vector<glm::vec3> offsets;

        constexpr int kk = 100;
        for (int i = -kk; i < kk; i++)
        {
            for (int j = -kk; j < kk; j++)
            {
                for (int k = -kk; k < kk; k++)
                {
                    if (rand() % 3 == 1)
                    {
                        offsets.push_back(glm::vec3(i, j, k));
                    }
                }
            }
        }

        LOG_DEBUG("Blocks: %1", offsets.size());

        test.Add<Index3DRender>(std::move(points), std::move(colors), std::move(indices), std::move(offsets));
        test.Get<Index3DRender>()->renderType = GL_TRIANGLE_FAN;

    }


    GLuint PRIMITIVE_RESTART = 12345; // magic value
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(PRIMITIVE_RESTART);
}

void RenderTestState::Update(TIMEDELTA dt)
{
    mSystems.UpdateAll(dt);
}

}; // namespace CubeWorld

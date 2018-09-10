// By Thomas Steinke

#include <cassert>
#include <functional>
#pragma warning(push, 0)
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>
#pragma warning(pop)

#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Shared/Components/CubeModel.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/Systems/AnimationSystem.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/FollowerSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/SimplePhysicsSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>
#include <Shared/Systems/WalkSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "StupidState.h"

namespace CubeWorld
{

namespace Game
{

   using Entity = Engine::Entity;
   using Transform = Engine::Transform;

   StupidState::StupidState(Engine::Window* window) : mWindow(window)
   {
      DebugHelper::Instance()->SetSystemManager(&mSystems);
      mSystems.Add<CameraSystem>(window->GetInput());
      mSystems.Add<AnimationSystem>(window->GetInput());
      mSystems.Add<FlySystem>(window->GetInput());
      mSystems.Add<WalkSystem>(window->GetInput());
      mSystems.Add<FollowerSystem>();
      mSystems.Add<MakeshiftSystem>();
      mSystems.Add<SimplePhysics::System>();
      mSystems.Add<SimplePhysics::Debug>(mSystems.Get<SimplePhysics::System>(), false, &mCamera);
      mSystems.Add<VoxelRenderSystem>(&mCamera);
      
      mSystems.Configure();
   }

   StupidState::~StupidState()
   {
      DebugHelper::Instance()->SetSystemManager(nullptr);
   }
   
   namespace {

   int index(int size, int i, int j)
   {
      return i * (2 * size + 1) + j;
   }

   }; // anonymous namespace

   bool StupidState::BuildFloorCollision(int32_t size)
   {
      static std::vector<bool> used(heights.size(), false);

      int blocksCreated = 0;

      auto index = [&](int i, int j) {
         return i * (2 * size + 1) + j;
      };

      auto makeCollider = [&](int i, int j, int height, int width, int length) {
         Entity collider = mEntities.Create(i - size + float(width - 1) / 2, float(height), j - size + float(length - 1) / 2);
         collider.Add<SimplePhysics::Collider>(glm::vec3(width, 1, length));

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

      LOG_INFO("Generated %1 blocks", blocksCreated);

      return false;
   }

   void StupidState::Start()
   {
      mWindow->GetInput()->SetMouseLock(true);
      
      Entity player = mEntities.Create();
      player.Add<Transform>(glm::vec3(0, 6, -10), glm::vec3(0, 0, 1));
      player.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
      player.Add<WalkSpeed>(10.0f, 3.0f, 15.0f);
      player.Add<SimplePhysics::Body>();
      player.Add<SimplePhysics::Collider>(glm::vec3(0.8f, 1.6f, 0.8f));
      
      Engine::ComponentHandle<AnimatedSkeleton> skeleton = player.Add<AnimatedSkeleton>(Asset::Animation("player.json"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"torso",1.0f}}, Asset::Model("body4.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"head",1.0f}}, Asset::Model("elf-head-m02.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"hair",1.0f}}, Asset::Model("elf-hair-m09.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"left_hand",1.0f}}, Asset::Model("hand2.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"right_hand",1.0f}}, Asset::Model("hand2.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"left_foot",1.0f}}, Asset::Model("foot.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"right_foot",1.0f}}, Asset::Model("foot.cub"));

      Entity playerCamera = mEntities.Create(0, 0, 0);
      ArmCamera::Options cameraOptions;
      cameraOptions.aspect = float(mWindow->Width()) / mWindow->Height();
      cameraOptions.far = 1500.0f;
      cameraOptions.distance = 3.5f;
      Engine::ComponentHandle<ArmCamera> handle = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
      playerCamera.Add<MouseControlledCamera>();
      playerCamera.Add<MouseControlledCameraArm>();
      playerCamera.Add<Follower>(player.Get<Transform>());
      player.Add<WalkDirector>(playerCamera.Get<Transform>(), false);

      mCamera.Set(handle.get());

      // Add some voxels.
      std::vector<Voxel::Data> carpet;
      std::vector<glm::vec3> points;
      std::vector<glm::vec3> colors;

      noise::module::Perlin heightmodule;
      heightmodule.SetFrequency(0.5);
      noise::utils::NoiseMap heightmap;
      noise::utils::NoiseMapBuilderPlane builder;
      builder.SetSourceModule(heightmodule);
      builder.SetDestNoiseMap(heightmap);
      const int size = 150;
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
            float elevation = 0.25f + 2 * pow(heightmap.GetValue(i + size, j + size), 2);
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

            glm::vec3 position = glm::vec3(i, std::round(elevation * 10), j);
            glm::vec4 color = dest * perc + source * (1 - perc);
            carpet.push_back(Voxel::Data(position, color, Voxel::All));

            heights[rowIndex + j + size] = int32_t(position.y);
         }
      }

      assert(carpet.size() > 0);

      Entity voxels = mEntities.Create(0, 0, 0);
      voxels.Add<VoxelRender>(std::move(carpet));

      BuildFloorCollision(size);
   }

}; // namespace Game

}; // namespace CubeWorld

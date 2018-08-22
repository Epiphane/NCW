// By Thomas Steinke

#include <cassert>
#include <functional>
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Game/Components/CubeModel.h>
#include <Game/Components/ArmCamera.h>
#include <Game/Systems/CameraSystem.h>
#include <Game/Systems/FlySystem.h>
#include <Game/Systems/MakeshiftSystem.h>
#include <Game/Systems/Simple3DRenderSystem.h>
#include <Game/Systems/SimplePhysicsSystem.h>
#include <Game/Systems/VoxelRenderSystem.h>

#include <Game/DebugHelper.h>
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
      mSystems.Add<CameraSystem>(Engine::Input::InputManager::Instance());
      mSystems.Add<FlySystem>(Engine::Input::InputManager::Instance());
      mSystems.Add<MakeshiftSystem>();
      mSystems.Add<SimplePhysics::System>();
      mSystems.Add<SimplePhysics::Debug>(mSystems.Get<SimplePhysics::System>(), false, &mCamera);
      mSystems.Add<VoxelRenderSystem>(&mCamera);
      
      mSystems.Configure();

      mEvents.Subscribe<NamedEvent>(*this);
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

   bool StupidState::BuildFloorCollision(int32_t size, const std::vector<int32_t>& heights)
   {
      static std::vector<bool> used(heights.size(), false);

      int blocksCreated = 0;

      auto index = [&](int i, int j) {
         return i * (2 * size + 1) + j;
      };

      auto makeCollider = [&](int i, int j, int height, int width, int length) {
         Entity collider = mEntities.Create(i - size + float(width - 1) / 2, height, j - size + float(length - 1) / 2);
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

            if (0) {
               // Attempt 1: Create squares only.
               // Result: Generated 3551 blocks
               int boxSize = 1, nextSize = 1;
               bool isDone = false;
               while (!isDone)
               {
                  boxSize = nextSize;
                  nextSize = boxSize + 1;
                  if (i + nextSize - 1 >= 2 * size + 1 || j + nextSize - 1 >= 2 * size + 1)
                  {
                     isDone = true;
                     break;
                  }

                  // First, check all the new boxes. I'm running out of bad letters
                  for (int n = 0; n < nextSize; ++n)
                  {
                     if (
                        used[index(i + nextSize - 1, j + n)] ||
                        used[index(i + n, j + nextSize - 1)] ||
                        heights[index(i + nextSize - 1, j + n)] != height ||
                        heights[index(i + n, j + nextSize - 1)] != height
                        )
                     {
                        isDone = true;
                        break;
                     }
                  }
               }

               if (++blocksCreated > mSkip) {
                  makeCollider(i, j, height, boxSize, boxSize);
                  mSkip++;
                  //return true;
               }
            }

            if (0) {
               // Attempt 2: Expand w, then h, then w, etc
               // Result: Generated 1691 blocks
               int width = 0, length = 0;
               int nWidth = 1, nLength = 1;
               bool isDone = false;
               while (nWidth > width || nLength > length)
               {
                  width = nWidth++;
                  length = nLength++;

                  if (j + nLength - 1 >= 2 * size + 1)
                  {
                     --nLength;
                  }
                  else
                  {
                     for (int n = 0; n < width; ++n)
                     {
                        if (
                           used[index(i + n, j + nLength - 1)] ||
                           heights[index(i + n, j + nLength - 1)] != height
                           )
                        {
                           --nLength;
                           break;
                        }
                     }
                  }

                  if (i + nWidth - 1 >= 2 * size + 1)
                  {
                     --nWidth;
                  }
                  else
                  {
                     for (int n = 0; n < nLength; ++n)
                     {
                        if (
                           used[index(i + nWidth - 1, j + n)] ||
                           heights[index(i + nWidth - 1, j + n)] != height
                           )
                        {
                           --nWidth;
                           break;
                        }
                     }
                  }
               }

               makeCollider(i, j, height, width, length);
               ++blocksCreated;
            }

            if (1) {
               // Attempt 3: Same as 2, but allow blocks to sit under each other
               // Result: Generated 933 blocks
               int width = 0, length = 0;
               int nWidth = 1, nLength = 1;
               bool isDone = false;
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
      }

      LOG_INFO("Generated %1 blocks", blocksCreated);

      return false;
   }

   void StupidState::Start()
   {
      Engine::Input::InputManager::Instance()->SetMouseLock(true);
      
      Entity player = mEntities.Create();
      player.Add<Transform>(glm::vec3(0, 2, -10), glm::vec3(0, 0, 1));
      player.Add<FlySpeed>(10);
      player.Add<SimplePhysics::Body>();
      player.Add<SimplePhysics::Collider>(glm::vec3(0.8));

      Entity playerCamera = mEntities.Create(0, 0, 0);
      playerCamera.Get<Transform>()->SetParent(player);
      ArmCamera::Options cameraOptions;
      cameraOptions.aspect = float(mWindow->Width()) / mWindow->Height();
      cameraOptions.far = 1500.0f;
      cameraOptions.distance = 3.0f;
      Engine::ComponentHandle<ArmCamera> handle = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
      playerCamera.Add<MouseControlledCamera>();
      playerCamera.Add<MouseControlledCameraArm>();

      Entity playerHead = mEntities.Create(0, 0, 0);
      playerHead.Get<Transform>()->SetParent(player);
      playerHead.Get<Transform>()->SetLocalScale(glm::vec3(0.1));
      playerHead.Add<CubeModel>("Models/head1.cub");

      player.Add<Makeshift>([&, player, playerCamera](Engine::EntityManager&, Engine::EventManager& events, TIMEDELTA) {
         player.Get<Transform>()->SetYaw(player.Get<Transform>()->GetYaw() + playerCamera.Get<Transform>()->GetYaw());
         playerCamera.Get<Transform>()->SetYaw(0);

         if (Engine::Input::InputManager::Instance()->IsKeyDown(GLFW_KEY_SPACE))
         {
            events.Emit(NamedEvent("whatever"));
         }
      });

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

      heights.empty();
      heights.resize(4 * (size + 1) * (size + 1));

      for (int i = -size; i <= size; ++i) {
         int rowIndex = (i + size) * (2 * size + 1);
         for (int j = -size; j <= size; ++j) {
            float elevation = pow(heightmap.GetValue(i + size, j + size), 2);
            glm::vec4 source, dest;
            double start, end;
            if (elevation >= 0.75) { source = ROCK; dest = SNOW; start = 0.75; end = 1.0; }
            else if (elevation >= 0.375) { source = DIRT; dest = ROCK; start = 0.375; end = 0.75; }
            else if (elevation >= 0.125) { source = GRASS; dest = DIRT; start = 0.125; end = 0.375; }
            else if (elevation >= 0.0625) { source = SAND; dest = GRASS; start = 0.0625; end = 0.125; }
            else if (elevation >= 0.0) { source = SHORE; dest = SAND; start = 0; end = 0.0625; }
            else if (elevation >= -0.25) { source = SHALLOW; dest = SHORE; start = -0.25; end = 0; }
            else { source = DEEP; dest = SHALLOW; start = -1.0; end = -0.25; }
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

      BuildFloorCollision(size, heights);
   }

   void StupidState::Receive(const NamedEvent& event)
   {
      const int size = 50;
      //BuildFloorCollision(size, heights);
   }

}; // namespace Game

}; // namespace CubeWorld

// By Thomas Steinke

#include <cassert>
#include <functional>
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Game/Components/CubeModel.h>
#include <Game/Components/ArmCamera.h>
#include <Game/Systems/AnimationSystem.h>
#include <Game/Systems/CameraSystem.h>
#include <Game/Systems/FlySystem.h>
#include <Game/Systems/MakeshiftSystem.h>
#include <Game/Systems/Simple3DRenderSystem.h>
#include <Game/Systems/SimplePhysicsSystem.h>
#include <Game/Systems/VoxelRenderSystem.h>

#include <Game/DebugHelper.h>
#include <Game/Helpers/Asset.h>
#include "AnimationStation.h"

namespace CubeWorld
{

namespace Game
{

   using Entity = Engine::Entity;
   using Transform = Engine::Transform;

   AnimationStation::AnimationStation(Engine::Window* window) : mWindow(window)
   {
      DebugHelper::Instance()->SetSystemManager(&mSystems);
      mSystems.Add<CameraSystem>(Engine::Input::InputManager::Instance());
      mSystems.Add<AnimationSystem>(Engine::Input::InputManager::Instance());
      mSystems.Add<MakeshiftSystem>();
      mSystems.Add<VoxelRenderSystem>(&mCamera);
      
      mSystems.Configure();

      mEvents.Subscribe<NamedEvent>(*this);
   }

   AnimationStation::~AnimationStation()
   {
      DebugHelper::Instance()->SetSystemManager(nullptr);
   }

   void AnimationStation::Start()
   {
      Engine::Input::InputManager::Instance()->SetMouseLock(true);
      
      Entity player = mEntities.Create();
      player.Add<Transform>(glm::vec3(0, 5, -10), glm::vec3(0, 0, 1));
      player.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
      player.Add<SimplePhysics::Body>();
      player.Add<SimplePhysics::Collider>(glm::vec3(0.8, 2.0, 0.8));
      Engine::ComponentHandle<AnimatedSkeleton> skeleton = player.Add<AnimatedSkeleton>(Asset::Animation("player.json"));

      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"torso",1.0f}}, Asset::Model("body4.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"head",1.0f}}, Asset::Model("elf-head-m02.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"hair",1.0f}}, Asset::Model("elf-hair-m09.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"left_hand",1.0f}}, Asset::Model("hand2.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"right_hand",1.0f}}, Asset::Model("hand2.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"left_foot",1.0f}}, Asset::Model("foot.cub"));
      skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"right_foot",1.0f}}, Asset::Model("foot.cub"));

      Entity playerCamera = mEntities.Create(0, 0, 0);
      playerCamera.Get<Transform>()->SetParent(player);
      playerCamera.Get<Transform>()->SetLocalScale(glm::vec3(10.0));
      ArmCamera::Options cameraOptions;
      cameraOptions.aspect = float(mWindow->Width()) / mWindow->Height();
      cameraOptions.far = 1500.0f;
      cameraOptions.distance = 3.5f;
      Engine::ComponentHandle<ArmCamera> handle = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
      playerCamera.Add<MouseControlledCamera>();
      playerCamera.Add<MouseControlledCameraArm>();

      player.Add<Makeshift>([&, player, playerCamera](Engine::EntityManager&, Engine::EventManager&, TIMEDELTA) {
      });

      Engine::Input::InputManager::Instance()->SetCallback(GLFW_KEY_SPACE, [player](){
         static int state = 0;
         std::vector<std::string> next = { "idle", "walk", "run", "walk" };
         state = (state + 1) % next.size();

         Engine::ComponentHandle<AnimatedSkeleton> skeleton = player.Get<AnimatedSkeleton>();
         double start = 0;
         if (state % 2 == 0) { start = skeleton->time / 2; }
         else if (state == 3) { start = skeleton->time * 2; }
         skeleton->TransitionTo(next[state], 0.5);
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

      heights.clear();
      heights.resize(4 * (size + 1) * (size + 1));

      for (int i = -size; i <= size; ++i) {
         int rowIndex = (i + size) * (2 * size + 1);
         for (int j = -size; j <= size; ++j) {
            double elevation = 0.25 + 2 * pow(heightmap.GetValue(i + size, j + size), 2);
            glm::vec4 source, dest;
            double start, end;
            if (elevation >= 0.75) { source = ROCK; dest = SNOW; start = 0.75; end = 1.0; }
            else if (elevation >= 0.375) { source = DIRT; dest = ROCK; start = 0.375; end = 0.75; }
            else if (elevation >= 0.125) { source = GRASS; dest = DIRT; start = 0.125; end = 0.375; }
            else if (elevation >= 0.0625) { source = SAND; dest = GRASS; start = 0.0625; end = 0.125; }
            else if (elevation >= 0.0) { source = SHORE; dest = SAND; start = 0; end = 0.0625; }
            else if (elevation >= -0.25) { source = SHALLOW; dest = SHORE; start = -0.25; end = 0; }
            else { source = DEEP; dest = SHALLOW; start = -1.0; end = -0.25; }
            float perc = float(elevation - start) / float(end - start);

            glm::vec3 position = glm::vec3(i, std::round(elevation * 10), j);
            glm::vec4 color = dest * perc + source * (1 - perc);
            carpet.push_back(Voxel::Data(position, color, Voxel::All));

            heights[rowIndex + j + size] = int32_t(position.y);
         }
      }

      assert(carpet.size() > 0);

      Entity voxels = mEntities.Create(0, 0, 0);
      voxels.Add<VoxelRender>(std::move(carpet));
   }

   void AnimationStation::Receive(const NamedEvent&)
   {
      const int size = 50;
      //BuildFloorCollision(size, heights);
   }

}; // namespace Game

}; // namespace CubeWorld

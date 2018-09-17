// By Thomas Steinke

#include <cassert>
#include <fstream>
#include <functional>
#pragma warning(push, 0)
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>
#pragma warning(pop)

#include <Engine/Core/Paths.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Engine/System/InputEventSystem.h>
#include <Shared/Components/CubeModel.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/SimplePhysicsSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#pragma warning(push, 0)
#include <Shared/Helpers/json.hpp>
#pragma warning(pop)
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

using Entity = Engine::Entity;
using Transform = Engine::Transform;

using ArmCamera = Game::ArmCamera;
using AnimatedSkeleton = Game::AnimatedSkeleton;
using DebugHelper = Game::DebugHelper;

MainState::MainState(Engine::Window* window, Bounded& parent)
   : mWindow(window)
   , mParent(parent)
   , mPlayer(Entity(nullptr, Entity::ID(0)))
{
   mFilename = Paths::Normalize(Asset::Animation("player.json"));
}

MainState::~MainState()
{
   DebugHelper::Instance()->SetSystemManager(nullptr);
}

void MainState::Initialize()
{
   // Create systems and configure
   DebugHelper::Instance()->SetSystemManager(&mSystems);
   mSystems.Add<Game::CameraSystem>(mWindow->GetInput());
   mSystems.Add<Game::AnimationSystem>(mWindow->GetInput());
   mSystems.Add<Game::MakeshiftSystem>();
   mSystems.Add<Game::VoxelRenderSystem>(&mCamera);
   mSystems.Configure();

   // Unlock the mouse
   mWindow->GetInput()->SetMouseLock(false);
      
   // Create a player component
   mPlayer = mEntities.Create();
   mPlayer.Add<Transform>(glm::vec3(0, 4.3, 0));
   mPlayer.Get<Transform>()->SetLocalScale(glm::vec3(0.1f));
   mPlayer.Add<AnimatedSkeleton>(mFilename);

   // Create a camera
   Entity playerCamera = mEntities.Create(0, 0, 0);
   playerCamera.Get<Transform>()->SetParent(mPlayer);
   playerCamera.Get<Transform>()->SetLocalScale(glm::vec3(10.0));
   playerCamera.Get<Transform>()->SetLocalDirection(glm::vec3(1, 0.5, -1));
   ArmCamera::Options cameraOptions;
   cameraOptions.aspect = float(mParent.GetWidth()) / mParent.GetHeight();
   cameraOptions.far = 1500.0f;
   cameraOptions.distance = 3.5f;
   Engine::ComponentHandle<ArmCamera> handle = playerCamera.Add<ArmCamera>(playerCamera.Get<Transform>(), cameraOptions);
   playerCamera.Add<Game::KeyControlledCamera>();
   playerCamera.Add<Game::KeyControlledCameraArm>();

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

   std::vector<int32_t> heights;
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
   voxels.Add<Game::VoxelRender>(std::move(carpet));
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

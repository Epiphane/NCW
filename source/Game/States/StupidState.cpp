// By Thomas Steinke

#include <cassert>
#include <functional>
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Game/Components/RenderCamera.h>
#include <Game/Systems/Simple3DRenderSystem.h>
#include <Game/Systems/VoxelRenderSystem.h>
#include <Game/Systems/CameraSystem.h>
#include <Game/Systems/FlySystem.h>

#include "StupidState.h"

namespace CubeWorld
{

namespace Game
{

   using Entity = Engine::Entity;
   using Transform = Engine::Transform;

   StupidState::StupidState(Engine::Window* window) : mWindow(window)
   {
      mSystems.Add<Simple3DRenderSystem>(&mCamera);
      mSystems.Add<VoxelRenderSystem>(&mCamera);
      mSystems.Add<CameraSystem>(Engine::Input::InputManager::Instance());
      mSystems.Add<FlySystem>(Engine::Input::InputManager::Instance());
      
      mSystems.Configure();
   }

   StupidState::~StupidState()
   {
   }

   void StupidState::Start()
   {
      Engine::Input::InputManager::Instance()->SetMouseLock(true);
      
      Entity player = mEntities.Create();
      player.Add<Transform>(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
      Component::RenderCamera::Options cameraOptions;
      cameraOptions.aspect = mWindow->Width() / mWindow->Height();
      cameraOptions.far = 200.0f;
      Engine::ComponentHandle<Component::RenderCamera> handle = player.Add<Component::RenderCamera>(
         player.Get<Transform>(),
         cameraOptions
      );
      player.Add<MouseControlledCamera>();
      player.Add<FlySpeed>(10);

      mCamera.Set(handle.get());

      // Add some voxels.
      std::vector<VoxelData> carpet;

      noise::module::Perlin heightmodule;
      noise::utils::NoiseMap heightmap;
      noise::utils::NoiseMapBuilderPlane builder;
      builder.SetSourceModule(heightmodule);
      builder.SetDestNoiseMap(heightmap);
      builder.SetDestSize(201, 201);
      const int size = 50;
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
     glm::vec4 DEEP(0, 0, 0.5f, 1);
     glm::vec4 SHALLOW(0, 0, 1, 1);
     glm::vec4 SHORE(0, 0.5f, 1, 1);
     glm::vec4 SAND(0.941f, 0.941f, 0.25, 1);
     glm::vec4 GRASS(0.125f, 0.625f, 0, 1);
     glm::vec4 DIRT(0.875f, 0.875f, 0, 1);
     glm::vec4 ROCK(0.5f, 0.5f, 0.5f, 1);
     glm::vec4 SNOW(1, 1, 1, 1);

      for (int i = -size; i <= size; ++i) {
         for (int j = -size; j <= size; ++j) {
            float elevation = heightmap.GetValue(i + size, j + size);
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

            glm::vec4 color = dest * perc + source * (1 - perc);
            carpet.push_back(VoxelData(glm::vec3(i, elevation * 5, j), color, VoxelSide::All));
         }
      }

      Entity voxels = mEntities.Create();
      voxels.Add<Transform>(glm::vec3(0, -1, -3));
      voxels.Add<VoxelRender>(std::move(carpet));
   }

}; // namespace Game

}; // namespace CubeWorld

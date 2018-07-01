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
      cameraOptions.aspect = float(mWindow->Width()) / mWindow->Height();
      cameraOptions.far = 500.0f;
      Engine::ComponentHandle<Component::RenderCamera> handle = player.Add<Component::RenderCamera>(
         player.Get<Transform>(),
         cameraOptions
      );
      player.Add<MouseControlledCamera>();
      player.Add<FlySpeed>(10);

      mCamera.Set(handle.get());

      // Add some voxels.
      std::vector<VoxelData> carpet;
      std::vector<glm::vec3> points;
      std::vector<glm::vec3> colors;

      noise::module::Perlin heightmodule;
      noise::utils::NoiseMap heightmap;
      noise::utils::NoiseMapBuilderPlane builder;
      builder.SetSourceModule(heightmodule);
      builder.SetDestNoiseMap(heightmap);
      const int size = 250;
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
      glm::vec4 DEEP(0, 0, 0.5f, 1);
      glm::vec4 SHALLOW(0, 0, 1, 1);
      glm::vec4 SHORE(0, 0.5f, 1, 1);
      glm::vec4 SAND(0.941f, 0.941f, 0.25, 1);
      glm::vec4 GRASS(0.125f, 0.625f, 0, 1);
      glm::vec4 DIRT(0.875f, 0.875f, 0, 1);
      glm::vec4 ROCK(0.5f, 0.5f, 0.5f, 1);
      glm::vec4 SNOW(1, 1, 1, 1);

      std::vector<GLfloat> p, c;
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

            glm::vec3 position = glm::vec3(i, std::round(elevation * 10), j);
            glm::vec4 color = dest * perc + source * (1 - perc);
            carpet.push_back(VoxelData(position, color, VoxelSide::Top | VoxelSide::Left | VoxelSide::Front));
         
            // Generate mesh automatically
            points.push_back(position + glm::vec3(-1, -1, -1));
            points.push_back(position + glm::vec3( 1, -1, -1));
            points.push_back(position + glm::vec3( 1,  1, -1));
            points.push_back(position + glm::vec3(-1, -1, -1));
            points.push_back(position + glm::vec3( 1,  1, -1));
            points.push_back(position + glm::vec3(-1,  1, -1));

            points.push_back(position + glm::vec3(-1, -1, -1));
            points.push_back(position + glm::vec3(-1, -1,  1));
            points.push_back(position + glm::vec3( 1, -1,  1));
            points.push_back(position + glm::vec3(-1, -1, -1));
            points.push_back(position + glm::vec3( 1, -1,  1));
            points.push_back(position + glm::vec3( 1, -1, -1));

            points.push_back(position + glm::vec3(-1, -1, -1));
            points.push_back(position + glm::vec3(-1,  1, -1));
            points.push_back(position + glm::vec3(-1,  1,  1));
            points.push_back(position + glm::vec3(-1, -1, -1));
            points.push_back(position + glm::vec3(-1,  1,  1));
            points.push_back(position + glm::vec3(-1, -1,  1));

            points.push_back(position + glm::vec3(-1, -1,  1));
            points.push_back(position + glm::vec3( 1,  1,  1));
            points.push_back(position + glm::vec3( 1, -1,  1));
            points.push_back(position + glm::vec3(-1, -1,  1));
            points.push_back(position + glm::vec3(-1,  1,  1));
            points.push_back(position + glm::vec3( 1,  1,  1));

            points.push_back(position + glm::vec3(-1,  1, -1));
            points.push_back(position + glm::vec3( 1,  1,  1));
            points.push_back(position + glm::vec3(-1,  1,  1));
            points.push_back(position + glm::vec3(-1,  1, -1));
            points.push_back(position + glm::vec3( 1,  1, -1));
            points.push_back(position + glm::vec3( 1,  1,  1));

            points.push_back(position + glm::vec3(-1, -1, -1));
            points.push_back(position + glm::vec3(-1,  1,  1));
            points.push_back(position + glm::vec3(-1,  1, -1));
            points.push_back(position + glm::vec3(-1, -1, -1));
            points.push_back(position + glm::vec3(-1, -1,  1));
            points.push_back(position + glm::vec3(-1,  1,  1));

            p.push_back(position.x - 1);
            p.push_back(position.y - 1);
            p.push_back(position.z);
            p.push_back(position.x + 1);
            p.push_back(position.y - 1);
            p.push_back(position.z);
            p.push_back(position.x - 1);
            p.push_back(position.y + 1);
            p.push_back(position.z);

            for (int k = 0; k < 36; k ++)
            {
               colors.push_back(color);
            }

            c.push_back(0.0f);
            c.push_back(0.0f);
            c.push_back(1.0f);
            c.push_back(1.0f);
            c.push_back(0.0f);
            c.push_back(0.0f);
            c.push_back(0.0f);
            c.push_back(1.0f);
            c.push_back(0.0f);
         }
      }

      /*for (glm::vec3 point : points) {
         p.push_back(point.x);
         p.push_back(point.y);
         p.push_back(point.z);
      }
      for (glm::vec3 color : colors) {
         c.push_back(color.x);
         c.push_back(color.y);
         c.push_back(color.z);
      }*/

      Entity voxels = mEntities.Create();
      voxels.Add<Transform>(glm::vec3(0, -1, 0));
      //voxels.Add<VoxelRender>(std::move(carpet));
      voxels.Add<Simple3DRender>(std::move(p), std::move(c));

      // Create some stupid component.
      Entity triangle = mEntities.Create();
      triangle.Add<Transform>(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0));
      triangle.Add<Simple3DRender>(std::vector<GLfloat>{
         -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         -1.0f, 1.0f, 0.0f,
      }, std::vector<GLfloat> {
         0.0f, 0.0f, 1.0f,
         1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
      });
      
      for (double i = 0; i < 2 * 3.14159; i += 3.14159/32)
      {
         Entity triangle2 = mEntities.Create();
         glm::vec3 pos = glm::vec3(sin(i), 0, cos(i));
         triangle2.Add<Transform>(10.0f * pos, -pos);
         triangle2.Add<Simple3DRender>(*triangle.Get<Simple3DRender>());

         Entity triangle3 = mEntities.Create();
         triangle3.Add<Transform>(10.0f * pos - glm::vec3(0, 1 + i/1000, 0), glm::vec3(0, 1, 0), -i);
         triangle3.Add<Simple3DRender>(*triangle.Get<Simple3DRender>());
      }
   }

}; // namespace Game

}; // namespace CubeWorld

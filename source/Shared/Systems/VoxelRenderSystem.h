// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <Engine/Core/Timer.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"
#include "../Voxel.h"

namespace CubeWorld
{

namespace Game
{

   struct VoxelRender : public Engine::Component<VoxelRender> {
      VoxelRender(Voxel::Model&& voxels);
      VoxelRender(const VoxelRender& other);
      
      Engine::Graphics::VBO mVoxelData;
      GLsizei mSize;
   };

   class VoxelRenderSystem : public Engine::System<VoxelRenderSystem> {
   public:
      VoxelRenderSystem(Engine::Graphics::Camera* camera = nullptr);
      ~VoxelRenderSystem();

      void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;

      void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

      void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }
      
   private:
      Engine::Graphics::Camera* mCamera;

      static GLuint program;
      static GLuint aPosition, aColor, aEnabledFaces;
      static GLuint uTint, uProjMatrix, uViewMatrix, uModelMatrix, uVoxelSize;

   private:
      std::unique_ptr<DebugHelper::MetricLink> metric;
      Engine::Timer<100> mClock;
   };

}; // namespace Game

}; // namespace CubeWorld

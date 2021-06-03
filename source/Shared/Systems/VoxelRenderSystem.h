// By Thomas Steinke

#pragma once

#include <GL/includes.h>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <Engine/Core/Timer.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"
#include "../Voxel.h"

namespace CubeWorld
{

struct VoxelRender : public Engine::Component<VoxelRender> {
   VoxelRender(std::vector<Voxel::Data>&& voxels);
   VoxelRender(const VoxelRender& other);

   void Set(const std::vector<Voxel::Data>& voxels);
   void Set(std::vector<Voxel::Data>&& voxels);

   Engine::Graphics::VBO mVoxelData;
   GLsizei mSize = 0;
};

class VoxelRenderSystem : public Engine::System<VoxelRenderSystem> {
public:
   VoxelRenderSystem(Engine::Graphics::Camera* camera = nullptr);
   ~VoxelRenderSystem();

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Reconfigure();

   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }
   
private:
   Engine::Graphics::Camera* mCamera;

   static std::unique_ptr<Engine::Graphics::Program> program;

private:
   Engine::Timer<100> mClock;
};

}; // namespace CubeWorld

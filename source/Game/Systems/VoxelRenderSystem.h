// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <Engine/Entity/EntityManager.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

namespace Game
{
   // Back
   enum VoxelSide {
      Back = 0x01,
      Bottom = 0x02,
      Left = 0x04,
      Front = 0x08,
      Top = 0x10,
      Right = 0x20,
      All = 0x3f
   };

   struct VoxelData {
      VoxelData(glm::vec3 position, glm::vec4 color, uint8_t faces = 0x3f)
         : position(position)
         , color(color)
         , enabledFaces(faces)
      {};

      glm::vec3 position;
      glm::vec3 color;
      uint8_t enabledFaces;
   };

   struct VoxelRender : public Engine::Component<VoxelRender> {
      VoxelRender(std::vector<VoxelData>&& voxels);
      VoxelRender(const VoxelRender& other);
      
      Engine::Graphics::VBO mVoxelData;
      uint32_t mSize;
   };

   class VoxelRenderSystem : public Engine::System<VoxelRenderSystem> {
   public:
      VoxelRenderSystem(Engine::Graphics::Camera* camera = nullptr);
      ~VoxelRenderSystem();

      void Configure(Engine::EntityManager& entities/*, EventManager& events*/) override;

      void Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt) override;

      void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }
      
   private:
      Engine::Graphics::Camera* mCamera;

      static GLuint program;
      static GLuint aPosition, aColor, aEnabledFaces;
      static GLuint uProjMatrix, uViewMatrix, uModelMatrix, uVoxelSize;
   };

}; // namespace Game

}; // namespace CubeWorld

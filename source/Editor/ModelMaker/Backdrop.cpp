// By Thomas Steinke

#pragma once

#include <Engine/Entity/Component.h>
#include <Engine/Event/Event.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Shared/Systems/VoxelRenderSystem.h>
#include <Shared/Components/CubeModel.h>

#include "Backdrop.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

Backdrop::Backdrop(Engine::ComponentHandle<Game::VoxelRender> component)
   : mComponent(component)
{
   mVoxels.clear();

   mComponent->Set(mVoxels);
}

void Backdrop::Receive(const ModelLoadedEvent& evt)
{
   mVoxels.clear();

   glm::vec3 size = glm::vec3(
      evt.component->mModel->mMetadata.width,
      evt.component->mModel->mMetadata.height,
      evt.component->mModel->mMetadata.length
   );
   glm::vec3 offset = size / -2.0f;

   // Colors
   const glm::vec4 DARK(18, 18, 18, 1);
   const glm::vec4 LIGHT(200, 200, 200, 1);
   for (uint32_t x = 0; x < size.x; ++x) {
      for (uint32_t z = 0; z < size.z; ++z) {
         glm::vec4 color = DARK;
         if (x % 2 == z % 2)
         {
            color = LIGHT;
         }

         glm::vec3 bottom = glm::vec3(x, -1, z);
         glm::vec3 top = glm::vec3(x, size.y, z);
         mVoxels.push_back(Voxel::Data(offset + bottom, color, Voxel::Top));
         mVoxels.push_back(Voxel::Data(offset + top, color, Voxel::Bottom));
      }
   }

   for (uint32_t y = 0; y < size.y; ++y) {
      for (uint32_t z = 0; z < size.z; ++z) {
         glm::vec4 color = DARK;
         if (y % 2 == z % 2)
         {
            color = LIGHT;
         }

         glm::vec3 left = glm::vec3(-1, y, z);
         glm::vec3 right = glm::vec3(size.x, y, z);
         mVoxels.push_back(Voxel::Data(offset + left, color, Voxel::Right));
         mVoxels.push_back(Voxel::Data(offset + right, color, Voxel::Left));
      }
   }

   for (uint32_t y = 0; y < size.y; ++y) {
      for (uint32_t x = 0; x < size.x; ++x) {
         glm::vec4 color = DARK;
         if (y % 2 == x % 2)
         {
            color = LIGHT;
         }

         glm::vec3 front = glm::vec3(x, y, size.z);
         glm::vec3 back = glm::vec3(x, y, -1);
         mVoxels.push_back(Voxel::Data(offset + front, color, Voxel::Back));
         mVoxels.push_back(Voxel::Data(offset + back, color, Voxel::Front));
      }
   }

   mComponent->Set(mVoxels);
}

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld

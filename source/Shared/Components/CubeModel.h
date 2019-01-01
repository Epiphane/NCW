// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>

#include <Engine/Entity/Component.h>
#include <Engine/Graphics/VBO.h>
#include "../Voxel.h"
#include "../Helpers/VoxFormat.h"

namespace CubeWorld
{

//
// CubModel represents a modeled Cube object, which contains any amount of voxels of varying colors.
//
class CubeModel : public Engine::Component<CubeModel> {
public:
   CubeModel();
   CubeModel(const std::string& path);
   CubeModel(const std::string& path, glm::vec3 tint);

   void Load(const std::string& path);
   void Load(const std::string& path, glm::vec3 tint);

   Voxel::Model* mModel;
   
   // Copy of the data from the model.
   Engine::Graphics::VBO mVBO;
   Voxel::Model::Metadata mMetadata;

   // To save space, mVoxelData isn't used if the model isn't modified. Instead, use mNumVoxels.
   std::vector<Voxel::Data> mVoxelData;
   size_t mNumVoxels;

   glm::vec3 mTint;
};

}; // namespace CubeWorld

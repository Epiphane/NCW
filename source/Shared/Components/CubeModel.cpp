// By Thomas Steinke

#include "CubeModel.h"

namespace CubeWorld
{

CubeModel::CubeModel()
   : mModel(nullptr)
   , mVBO(Engine::Graphics::VBO::Vertices)
   , mMetadata{0, 0, 0}
   , mNumVoxels(0)
   , mTint(glm::vec3(255))
{
}

CubeModel::CubeModel(const std::string& path)
{
   Load(path);
}

CubeModel::CubeModel(const std::string& path, glm::vec3 tint)
{
   Load(path, tint);
}

void CubeModel::Load(const std::string& path)
{
   mModel = Voxel::VoxFormat::Load(path, false);
   assert(mModel != nullptr);
   mNumVoxels = mModel->mVoxelData.size();
   mMetadata = mModel->mMetadata;
   mVBO = mModel->mVBO;
   mTint = glm::vec3(255);
}

void CubeModel::Load(const std::string& path, glm::vec3 tint)
{
   mModel = Voxel::VoxFormat::Load(path, true);
   assert(mModel != nullptr);
   mNumVoxels = mModel->mVoxelData.size();
   mMetadata = mModel->mMetadata;
   mVBO = mModel->mVBO;
   mTint = tint;
}

}; // namespace CubeWorld

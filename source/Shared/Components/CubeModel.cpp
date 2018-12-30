// By Thomas Steinke

#if CUBEWORLD_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdio>
#include <fstream>
#include <glad/glad.h>

#include <Engine/Core/Scope.h>
#include <Engine/Logger/Logger.h>
#include "../Voxel.h"

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
   mModel = Voxel::CubeFormat::Load(path, false);
   assert(mModel != nullptr);
   mNumVoxels = mModel->mVoxelData.size();
   mMetadata = mModel->mMetadata;
   mVBO = mModel->mVBO;
   mTint = glm::vec3(255);
}

void CubeModel::Load(const std::string& path, glm::vec3 tint)
{
   mModel = Voxel::CubeFormat::Load(path, true);
   assert(mModel != nullptr);
   mNumVoxels = mModel->mVoxelData.size();
   mMetadata = mModel->mMetadata;
   mVBO = mModel->mVBO;
   mTint = tint;
}

}; // namespace CubeWorld

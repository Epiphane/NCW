// By Thomas Steinke

#if CUBEWORLD_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdio>
#include <fstream>
#include <GL/glew.h>

#include <Engine/Core/Scope.h>
#include <Engine/Logger/Logger.h>
#include "../Voxel.h"

#include "CubeModel.h"

namespace CubeWorld
{

namespace Game
{

CubeModel::CubeModel(const std::string& path)
{
   mModel = CubeModelInfo::Load(path, false);
   assert(mModel != nullptr);
   mNumVoxels = mModel->mVoxelData.size();
   mMetadata = mModel->mMetadata;
   mVBO = mModel->mVBO;
   mTint = glm::vec3(255);
}

CubeModel::CubeModel(const std::string& path, glm::vec3 tint)
{
   mModel = CubeModelInfo::Load(path, true);
   assert(mModel != nullptr);
   mNumVoxels = mModel->mVoxelData.size();
   mMetadata = mModel->mMetadata;
   mVBO = mModel->mVBO;
   mTint = tint;
}

std::unordered_map<std::string, std::unique_ptr<CubeModelInfo>> CubeModelInfo::sModels;

CubeModelInfo::CubeModelInfo() : mVBO(Engine::Graphics::VBO::Vertices)
{}

CubeModelInfo::~CubeModelInfo()
{}

namespace
{

bool IsFilled(const std::vector<bool>& filled, int index)
{
   if (index < 0 || index >= filled.size())
   {
      return false;
   }

   return filled[index];
}

int Index(const CubeModelInfo::FileHeader& metadata, int x, int y, int z)
{
   if (x < 0 || y < 0 || z < 0) { return -1; }
   if (x >= metadata.width || y >= metadata.height || z >= metadata.length) { return -1; }
   return x + z * metadata.width + y * metadata.width * metadata.length;
}

uint8_t GetExposedFaces(const std::vector<bool>& filled, const CubeModelInfo::FileHeader& metadata, int x, int y, int z)
{
   uint8_t faces = Voxel::All;
   int right = Index(metadata, x - 1, y, z);
   int left = Index(metadata, x + 1, y, z);
   int front = Index(metadata, x, y, z + 1);
   int behind = Index(metadata, x, y, z - 1);
   int above = Index(metadata, x, y + 1, z);
   int below = Index(metadata, x, y - 1, z);

   // Check overflows
   if (right % metadata.width == 0) { right = -1; }
   if (front % (metadata.width * metadata.height) == 0) { front = -1; }

   if (IsFilled(filled, right)) { faces ^= Voxel::Right; }
   if (IsFilled(filled, left)) { faces ^= Voxel::Left; }
   if (IsFilled(filled, front)) { faces ^= Voxel::Front; }
   if (IsFilled(filled, behind)) { faces ^= Voxel::Back; }
   if (IsFilled(filled, above)) { faces ^= Voxel::Top; }
   if (IsFilled(filled, below)) { faces ^= Voxel::Bottom; }

   return faces;
}

}; // namespace

CubeModelInfo* CubeModelInfo::Load(const std::string& path, bool tintable)
{
   auto maybeModel = sModels.find(path);
   if (maybeModel != sModels.end())
   {
      assert(tintable == maybeModel->second->mIsTintable);

      return maybeModel->second.get();
   }

   std::unique_ptr<CubeModelInfo> result = std::make_unique<CubeModelInfo>();

   FILE* f = fopen(path.c_str(), "rb");
   if (f == nullptr)
   {
      LOG_ERROR("Failed opening %1 with errno %2", path, errno);
      return nullptr;
   }

   CUBEWORLD_SCOPE_EXIT([&] { fclose(f); })

   size_t bytes = fread(&result->mMetadata, 1, sizeof(FileHeader), f);
   if (bytes != sizeof(FileHeader))
   {
      LOG_ERROR("Failed to read %1 bytes: got %2 instead.", sizeof(FileHeader), bytes);
      return nullptr;
   }

   assert(result->mMetadata.width > 0);
   assert(result->mMetadata.length > 0);
   assert(result->mMetadata.height > 0);

   std::vector<uint8_t> data;
   std::vector<bool> filled;

   uint32_t nElements = result->mMetadata.width * result->mMetadata.length * result->mMetadata.height;
   data.resize(3 * nElements);
   filled.resize(nElements);

   size_t numRead = fread(&data[0], sizeof(uint8_t), 3 * nElements, f);
   if (numRead != 3 * nElements)
   {
      LOG_ERROR("Failed to read %1 elements: got %2 instead.", 3 * nElements, numRead);
      return nullptr;
   }

   for (uint32_t i = 0; i < nElements; i++)
   {
      if (tintable)
      {
         // Ignored blocks are defined by being (255,0,0), (0,255,0), or (0,0,255).
         filled[i] = !(data[3 * i] == 0 || data[3 * i + 1] == 0 || data[3 * i + 2] == 0);
      }
      else
      {
         // Anything that's not pure black is valid.
         filled[i] = !(data[3 * i] == 0 && data[3 * i + 1] == 0 && data[3 * i + 2] == 0);
      }
   }

   // Now, construct the voxel data.
   Voxel::Data voxel;
   for (uint32_t y = 0; y < result->mMetadata.height; y++)
   {
      for (uint32_t z = 0; z < result->mMetadata.length; z++)
      {
         for (uint32_t x = 0; x < result->mMetadata.width; x++)
         {
            int ndx = Index(result->mMetadata, x, y, z);
            if (filled[ndx])
            {
               // Active voxel
               voxel.color.r = data[3 * ndx];
               voxel.color.g = data[3 * ndx + 1];
               voxel.color.b = data[3 * ndx + 2];
               voxel.enabledFaces = GetExposedFaces(filled, result->mMetadata, x, y, z);
               voxel.position.x = result->mMetadata.width / 2 - float(x);
               voxel.position.y = float(y) - result->mMetadata.height / 2;
               voxel.position.z = float(z) - result->mMetadata.length / 2;
               result->mVoxelData.push_back(voxel);
            }
         }
      }
   }

   result->mVBO.BufferData(sizeof(Voxel::Data) * int(result->mVoxelData.size()), &result->mVoxelData[0], GL_STATIC_DRAW);
   result->mIsTintable = tintable;

   auto emplaceResult = sModels.emplace(path, std::move(result));
   return emplaceResult.first->second.get();
}
   
}; // namespace Game

}; // namespace CubeWorld

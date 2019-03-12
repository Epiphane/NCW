// By Thomas Steinke

#if CUBEWORLD_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdio>
#include <errno.h>

#include <Engine/Core/Scope.h>
#include <Engine/Logger/Logger.h>

#include "CubeFormat.h"

namespace CubeWorld
{

namespace Voxel
{

std::unordered_map<std::string, std::unique_ptr<Model>> CubeFormat::sModels;

namespace
{

bool IsFilled(const std::vector<bool>& filled, int32_t index)
{
   if (index < 0 || index >= filled.size())
   {
      return false;
   }

   return filled[index];
}

int32_t Index(const Model::Metadata& metadata, uint32_t x, uint32_t y, uint32_t z)
{
   if (x >= metadata.width || y >= metadata.height || z >= metadata.length) { return -1; }
   return x + z * metadata.width + y * metadata.width * metadata.length;
}

uint8_t GetExposedFaces(const std::vector<bool>& filled, const Model::Metadata& metadata, uint32_t x, uint32_t y, uint32_t z)
{
   uint8_t faces = All;
   int32_t right = Index(metadata, x - 1, y, z);
   int32_t left = Index(metadata, x + 1, y, z);
   int32_t front = Index(metadata, x, y, z + 1);
   int32_t behind = Index(metadata, x, y, z - 1);
   int32_t above = Index(metadata, x, y + 1, z);
   int32_t below = Index(metadata, x, y - 1, z);

   // Check overflows
   if (right % metadata.width == 0) { right = -1; }
   if (front % (metadata.width * metadata.height) == 0) { front = -1; }

   if (IsFilled(filled, right)) { faces ^= Right; }
   if (IsFilled(filled, left)) { faces ^= Left; }
   if (IsFilled(filled, front)) { faces ^= Front; }
   if (IsFilled(filled, behind)) { faces ^= Back; }
   if (IsFilled(filled, above)) { faces ^= Top; }
   if (IsFilled(filled, below)) { faces ^= Bottom; }

   return faces;
}

}; // namespace

Model* CubeFormat::Load(const std::string& path, bool tintable)
{
   auto maybeModel = sModels.find(path);
   if (maybeModel != sModels.end())
   {
      assert(tintable == maybeModel->second->mIsTintable);

      return maybeModel->second.get();
   }

   Maybe<std::unique_ptr<ModelData>> result = Read(path, tintable);
   if (!result)
   {
      LOG_ERROR(result.Failure().WithContext("Failed loading model").GetMessage());
      return nullptr;
   }
   std::unique_ptr<Model> model = std::make_unique<Model>(std::move(result.Result()));

   model->mVBO.BufferData(sizeof(Data) * int(model->mVoxelData.size()), &model->mVoxelData[0], GL_STATIC_DRAW);
   model->mIsTintable = tintable;

   auto emplaceResult = sModels.emplace(path, std::move(model));
   return emplaceResult.first->second.get();
}

Maybe<std::unique_ptr<ModelData>> CubeFormat::Read(const std::string& path, bool tintable)
{
   std::unique_ptr<ModelData> result = std::make_unique<ModelData>();

   FILE* f = fopen(path.c_str(), "rb");
   if (f == nullptr)
   {
      return Failure("Failed opening %1 with errno %2", path, errno);
   }

   CUBEWORLD_SCOPE_EXIT([&] { fclose(f); })

   size_t bytes = fread(&result->mMetadata, 1, sizeof(ModelData::Metadata), f);
   if (bytes != sizeof(ModelData::Metadata))
   {
      return Failure("Failed to read %1 bytes: got %2 instead.", sizeof(ModelData::Metadata), bytes);
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
      return Failure("Failed to read %1 elements: got %2 instead.", 3 * nElements, numRead);
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
   // According to the format, the first {height} values are RGB values for the voxel at {x, 0, 0}, the front bottom row.
   // Following that, the next {height} values are for the voxels at {x, 0, 1} ... {x, 0, length - 1}, the bottom face.
   // Each {length*height} subsection contains a new layer on top of the previous, starting at the bottom.
   Data voxel;
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

   return std::move(result);
}

Maybe<void> CubeFormat::Write(const std::string& path, const ModelData& model)
{
   // Validate input. Sometimes mVoxelData is optimized out, so make sure
   // it exists.
   if (model.mVoxelData.size() == 0)
   {
      return Failure{"Voxel data provided was empty."};
   }

   FILE* f = fopen(path.c_str(), "wb");
   if (f == nullptr)
   {
      return Failure("Failed opening %1 with errno %2", path, errno);
   }

   CUBEWORLD_SCOPE_EXIT([&] { fclose(f); });

   size_t bytes = fwrite(&model.mMetadata, 1, sizeof(Model::Metadata), f);
   if (bytes != sizeof(Model::Metadata))
   {
      return Failure("Failed to write %1-byte header: wrote %2 instead.", sizeof(Model::Metadata), bytes);
   }

   std::vector<uint8_t> data;
   data.resize(3 * model.mMetadata.width * model.mMetadata.height * model.mMetadata.length, 0);

   // Now, unpack the voxel data.
   // Unpack values in the same order they are created. This does NOT line up with the
   // format of the data on-disk (see the note above for disk layout).
   for (const Data& voxel : model.mVoxelData)
   {
      // Inverse of how we do centering.
      uint32_t x = model.mMetadata.width / 2 - uint32_t(voxel.position.x);
      uint32_t y = uint32_t(voxel.position.y) + model.mMetadata.height / 2;
      uint32_t z = uint32_t(voxel.position.z) + model.mMetadata.length / 2;
      int32_t ndx = Index(model.mMetadata, x, y, z);
      if (ndx < 0)
      {
         return Failure("Attempting to save voxel at position {%1, %2, %3} even though the size is only %4x%5x%6",
            x, y, z,
            model.mMetadata.width,
            model.mMetadata.height,
            model.mMetadata.length);
      }

      data[3 * ndx] = uint8_t(voxel.color.r);
      data[3 * ndx + 1] = uint8_t(voxel.color.g);
      data[3 * ndx + 2] = uint8_t(voxel.color.b);
   }

   size_t written = fwrite(&data[0], sizeof(std::vector<uint8_t>::value_type), data.size(), f);
   if (written != data.size())
   {
      return Failure("Failed to write %1 elements: wrote %2 instead.", data.size(), written);
   }

   return Success;
}

}; // namespace Voxel

}; // namespace CubeWorld
// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>

#include <Engine/Entity/Component.h>
#include <Engine/Graphics/VBO.h>
#include "../Voxel.h"

namespace CubeWorld
{

namespace Game
{

   class CubeModelInfo {
   public:
      // File format for the .cub file
      struct FileHeader {
         uint32_t width;
         uint32_t length;
         uint32_t height;
      };

   public:
      CubeModelInfo();
      ~CubeModelInfo();

      static CubeModelInfo* Load(const std::string& path, bool tintable);

   public:
      std::vector<Voxel::Data> mVoxelData;
      Engine::Graphics::VBO mVBO;
      FileHeader mMetadata;
      bool mIsTintable;

   private:
      friend class CubeModel;
      static std::unordered_map<std::string, std::unique_ptr<CubeModelInfo>> sModels;
   };

   //
   // CubModel represents a modeled Cube object, which contains any amount of voxels of varying colors.
   //
   class CubeModel : public Engine::Component<CubeModel> {
   public:
      CubeModel(const std::string& path);
      CubeModel(const std::string& path, glm::vec3 tint);

      CubeModelInfo* mModel;
     
      // Copy of the data from the model.
      Engine::Graphics::VBO mVBO;
      CubeModelInfo::FileHeader mMetadata;

      // To save space, mVoxelData isn't used if the model isn't modified. Instead, use mNumVoxels.
      std::vector<Voxel::Data> mVoxelData;
      size_t mNumVoxels;

      glm::vec3 mTint;
   };

}; // namespace Game

}; // namespace CubeWorld

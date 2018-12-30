// By Thomas Steinke

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <Engine/Graphics/VBO.h>

namespace CubeWorld
{

namespace Voxel
{

enum Side {
   Back = 0x01,
   Bottom = 0x02,
   Left = 0x04,
   Front = 0x08,
   Top = 0x10,
   Right = 0x20,
   All = 0x3f
};

struct Data {
   Data() : position(0, 0, 0), color(0, 0, 0), enabledFaces(0) {};
   Data(glm::vec3 position, glm::vec4 color, uint8_t faces = 0x3f)
      : position(position)
      , color(color)
      , enabledFaces(faces)
   {};

   glm::vec3 position;
   glm::vec3 color;
   uint8_t enabledFaces;
};

class ModelData {
public:
   struct Metadata {
      uint32_t width; // x
      uint32_t length; // z
      uint32_t height; // y
   };

   ModelData() : mVoxelData{}, mMetadata{0, 0, 0}, mIsTintable{false} {};
   ModelData(const ModelData& other)
      : mVoxelData(other.mVoxelData)
      , mMetadata(other.mMetadata)
      , mIsTintable(other.mIsTintable)
   {};
   ModelData(ModelData&& other)
      : mVoxelData(std::move(other.mVoxelData))
      , mMetadata(other.mMetadata)
      , mIsTintable(other.mIsTintable)
   {};

public:
   std::vector<Data> mVoxelData;
   Metadata mMetadata;
   bool mIsTintable;
};

// Extension of ModelData that provides a VBO.
class Model : public ModelData {
public:
   Model() : mVBO(Engine::Graphics::VBO::Vertices) {};
   Model(std::unique_ptr<ModelData>&& data)
      : ModelData(std::move(*data))
      , mVBO(Engine::Graphics::VBO::Vertices)
   {};
   Model(ModelData&& data)
      : ModelData(std::move(data))
      , mVBO(Engine::Graphics::VBO::Vertices)
   {};
   ~Model() {};

   Engine::Graphics::VBO mVBO;
};

}; // namespace Voxel

}; // namespace CubeWorld

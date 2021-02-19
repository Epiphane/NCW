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
    Data() : position(0, 0, 0), color(0, 0, 0), enabledFaces(0), occlusion(0) {};
    Data(glm::vec3 position, glm::vec4 color, uint8_t faces = 0x3f, uint32_t occlusion = 0)
        : position(position)
        , color(color)
        , enabledFaces(faces)
        , occlusion(occlusion)
    {};

    glm::vec3 position;
    glm::vec3 color;
    uint8_t enabledFaces;

    // Integer for computing ambient occlusion.
    //
    // Format is a packed set of 8 3-bit numbers, representing
    // how many voxels neighbor each vertex on this cube.
    //
    // For example, consider the following:
    // 0b 0000 0001 0010 0011 0100 0101 0110 0111
    // These correspond to the following vertices, in order:
    // (0, 0, 0) [000]: 0 neighbors (pure color)
    // (0, 0, 1) [001]: 1 neighbor
    // (0, 1, 0) [010]: 2 neighbors
    // (0, 1, 1) [011]: 3 neighbors
    // (1, 0, 0) [100]: 4 neighbors
    // (1, 0, 1) [101]: 5 neighbors
    // (1, 1, 0) [110]: 6 neighbors (maximum, very dark)
    // (1, 1, 1) [111]: 7 neighbors (impossible, zero light)
    uint32_t occlusion;
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
   ModelData(ModelData&& other) noexcept
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

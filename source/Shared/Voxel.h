// By Thomas Steinke

#pragma once

#include <vector>
#include <glm/glm.hpp>

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

typedef std::vector<Data> Model;

}; // namespace Voxel

}; // namespace CubeWorld

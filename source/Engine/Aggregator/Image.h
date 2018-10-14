// By Thomas Steinke

#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include <unordered_map>

#include "../Graphics/FontManager.h"
#include "Aggregator.h"

namespace CubeWorld
{

namespace Engine
{

namespace Aggregator
{

struct ImageData
{
   glm::vec3 position;
   glm::vec2 uv;
};

//
// Actual aggregator. There will be one per texture.
//
class Image : public Aggregator<ImageData>
{
public:
   Image();

   void ConnectToTexture(const Region& region, GLuint texture);

   void Render() override;

private:
   // Map from Texture ID to a list of indices in mVBO.
   using VBOWithData = std::pair<Graphics::VBO, std::vector<GLuint>>;
   std::unordered_map<GLuint, VBOWithData> mTextureIndices;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Aggregator
   
}; // namespace Engine

}; // namespace CubeWorld

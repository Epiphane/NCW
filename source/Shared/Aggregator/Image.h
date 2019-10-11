// By Thomas Steinke

#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include <unordered_map>

#include <Engine/Graphics/Program.h>
#include <Engine/Aggregator/Aggregator.h>

namespace CubeWorld
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
class Image : public Engine::Aggregator<ImageData>
{
public:
   Image(Bounded* bounds);

   void ConnectToTexture(const Region& region, GLuint texture);

   void Render() override;

private:
   // Map from Texture ID to a list of indices in mVBO.
   using VBOWithData = std::pair<Engine::Graphics::VBO, std::vector<GLuint>>;
   std::unordered_map<GLuint, VBOWithData> mTextureIndices;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Aggregator
   
}; // namespace CubeWorld

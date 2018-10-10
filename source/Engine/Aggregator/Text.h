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

struct TextData
{
   glm::vec2 position;
   glm::vec2 uv;
};

//
// Actual aggregator. There will be one per texture.
//
class Text : public Aggregator<TextData>
{
public:
   Text();

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

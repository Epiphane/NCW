// By Thomas Steinke

#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include <unordered_map>

#include "../Graphics/FontManager.h"
#include "../Graphics/Program.h"
#include "../Graphics/VBO.h"
#include "UIAggregator.h"

namespace CubeWorld
{

namespace Engine
{

//
// Actual aggregator. There will be one per texture.
//
class TextAggregator : public UIAggregator<Graphics::Font::CharacterVertexUV>
{
public:
   TextAggregator();

   void ConnectToTexture(const Region& region, GLuint texture);

   void Render() override;

private:
   // Map from Texture ID to a list of indices in mVBO.
   using VBOWithData = std::pair<Graphics::VBO, std::vector<GLuint>>;
   std::unordered_map<GLuint, VBOWithData> mTextureIndices;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};
   
}; // namespace Engine

}; // namespace CubeWorld

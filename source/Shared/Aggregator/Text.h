// By Thomas Steinke

#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include <unordered_map>

#include <Engine/Graphics/FontManager.h>
#include <Engine/Aggregator/Aggregator.h>

namespace CubeWorld
{

namespace Aggregator
{

struct TextData
{
   glm::vec3 position;
   glm::vec2 uv;
};

//
// Actual aggregator. There will be one per texture.
//
class Text : public Engine::Aggregator<TextData>
{
public:
   Text(Engine::UIRoot* root);

   void ConnectToTexture(const Region& region, GLuint texture);

   void Render() override;

private:
   struct VBOWithData
   {
      Engine::Graphics::VBO vbo;
      std::vector<GLuint> data;
      bool dirty;
   };

   // Map from Texture ID to a list of indices in mVBO.
   std::unordered_map<GLuint, VBOWithData> mTextureIndices;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Aggregator

}; // namespace CubeWorld

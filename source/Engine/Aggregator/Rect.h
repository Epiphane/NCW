// By Thomas Steinke

#pragma once

#include <algorithm>
#include <cstring>
#include <glm/glm.hpp>
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

struct RectData {
   glm::vec2 position;
   glm::vec4 color;
};

class Rect : public Aggregator<RectData>
{
public:
   Rect();

   void Render() override;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Aggregator
   
}; // namespace Engine

}; // namespace CubeWorld

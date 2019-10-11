// By Thomas Steinke

#pragma once

#include <algorithm>
#include <cstring>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

#include <Engine/Graphics/Program.h>
#include <Engine/Aggregator/Aggregator.h>

namespace CubeWorld
{

namespace Aggregator
{

struct RectData {
   glm::vec3 position;
   glm::vec4 color;
};

class Rect : public Engine::Aggregator<RectData>
{
public:
   Rect(Bounded* bounds);

   void Render() override;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Aggregator

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

class Renderer
{
protected:
   uint64_t mElements;

public:
   Renderer() : mElements(0) {};
   virtual ~Renderer() {};

   uint64_t GetNumElements() { return mElements; }
   void SetNumElements(uint32_t num) { mElements = num; }

   virtual void Render(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) = 0;

   virtual std::unique_ptr<Renderer> Clone() = 0;
};

}; // namespace Engine

}; // namespace Graphics

}; // namespace CubeWorld

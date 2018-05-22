// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Graphics/Renderer.h>
#include <Engine/Graphics/VBO.h>

namespace CubeWorld
{

namespace Game
{

class StupidRenderer : public Engine::Graphics::Renderer {
public:
   StupidRenderer();
   ~StupidRenderer();

   void Render(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) override;

   std::unique_ptr<Renderer> Clone() override;

private:
   std::unique_ptr<Engine::Graphics::VBO> mVertices, mColors;

   static void Initalize();
   static GLuint program;
   static GLuint aPosition, aColor;
   static GLuint uProjMatrix, uViewMatrix, uModelMatrix;
};

}; // namespace Game

}; // namespace CubeWorld

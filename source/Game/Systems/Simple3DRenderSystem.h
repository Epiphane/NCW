// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <memory>
#include <vector>

#include <Engine/Entity/EntityManager.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

namespace Game
{

struct Simple3DRender : public Engine::Component<Simple3DRender> {
   Simple3DRender(std::vector<GLfloat>&& points, std::vector<GLfloat>&& colors)
      : points(points)
      , colors(colors)
   {};

   std::vector<GLfloat> points;
   std::vector<GLfloat> colors;
};

class Simple3DRenderSystem : public Engine::System<Simple3DRenderSystem> {
public:
   Simple3DRenderSystem(Engine::Graphics::Camera* camera);
   ~Simple3DRenderSystem();

   void Configure(Engine::EntityManager& entities/*, EventManager& events*/) override;

   void Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt) override;

private:
   Engine::Graphics::Camera* mCamera;

   std::unique_ptr<Engine::Graphics::VBO> mVertices, mColors;

   static GLuint program;
   static GLuint aPosition, aColor;
   static GLuint uProjMatrix, uViewMatrix, uModelMatrix;
};

}; // namespace Game

}; // namespace CubeWorld

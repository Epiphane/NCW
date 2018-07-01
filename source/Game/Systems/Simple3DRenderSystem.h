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
   
   class Simple3DComponentCamera : public Engine::Graphics::Camera {
      Engine::ComponentHandle<Engine::Transform> transform;
   };

   struct Simple3DRender : public Engine::Component<Simple3DRender> {
      Simple3DRender(std::vector<GLfloat>&& points, std::vector<GLfloat>&& colors);
      Simple3DRender(const Simple3DRender& other);
      
      Engine::Graphics::VBO mVertices, mColors;
   };

   class Simple3DRenderSystem : public Engine::System<Simple3DRenderSystem> {
   public:
      Simple3DRenderSystem(Engine::Graphics::Camera* camera = nullptr);
      ~Simple3DRenderSystem();

      void Configure(Engine::EntityManager& entities/*, EventManager& events*/) override;

      void Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt) override;

      void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }
      
   private:
      Engine::Graphics::Camera* mCamera;

      static GLuint program;
      static GLuint aPosition, aColor;
      static GLuint uProjMatrix, uViewMatrix, uModelMatrix;
   };

}; // namespace Game

}; // namespace CubeWorld

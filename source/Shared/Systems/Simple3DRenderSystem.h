// By Thomas Steinke

#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>

#include <Engine/Core/Timer.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"

namespace CubeWorld
{

class Simple3DComponentCamera : public Engine::Graphics::Camera {
   Engine::ComponentHandle<Engine::Transform> transform;
};

struct Simple3DRender : public Engine::Component<Simple3DRender> {
   Simple3DRender();
   Simple3DRender(std::vector<GLfloat>&& points, std::vector<GLfloat>&& colors);
   Simple3DRender(const Simple3DRender& other);
   
   Engine::Graphics::VBO mVertices, mColors;
   size_t mCount;
   bool cullFaces = true;
};

class Simple3DRenderSystem : public Engine::System<Simple3DRenderSystem> {
public:
   Simple3DRenderSystem(Engine::Graphics::Camera* camera = nullptr);
   ~Simple3DRenderSystem();

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;

   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }
   
private:
   Engine::Graphics::Camera* mCamera;

   static std::unique_ptr<Engine::Graphics::Program> program;

private:
   std::unique_ptr<DebugHelper::MetricLink> metric;
   Engine::Timer<100> mClock;
};

}; // namespace CubeWorld

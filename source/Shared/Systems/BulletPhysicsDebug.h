// By Thomas Steinke

#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

#include <Engine/Core/Timer.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"
#include "BulletPhysicsSystem.h"

namespace CubeWorld
{

namespace BulletPhysics
{

//
// Debug draws collision objects, for debugging physics.
//
class Debug : public Engine::System<Debug>, public btIDebugDraw {
public:
   Debug(BulletPhysics::System* system, Engine::Graphics::Camera* camera = nullptr);
   ~Debug() {}

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }

public:
   // btIDebugDraw implementation
   void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
   void flushLines() override;
   void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
   void reportErrorWarning(const char* warningString) override;
   void draw3dText(const btVector3& location, const char* textString) override;
   void setDebugMode(int debugMode) override;
   int getDebugMode() const override;

private:
   BulletPhysics::System* mSystem;
   Engine::Graphics::Camera* mCamera;

   glm::vec3 mColor;
   std::vector<glm::vec3> mPoints;
   Engine::Graphics::VBO mVBO;

   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace BulletPhysics

}; // namespace CubeWorld

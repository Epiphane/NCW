// By Thomas Steinke

#include <deque>
#include <glad/glad.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <RGBDesignPatterns/Macros.h>
#include <RGBDesignPatterns/Scope.h>
#include <RGBLogger/Logger.h>
#include <Engine/Graphics/Program.h>

#include "../Helpers/Asset.h"
#include "BulletPhysicsDebug.h"

namespace CubeWorld
{

namespace BulletPhysics
{

///
///
///
std::unique_ptr<Engine::Graphics::Program> Debug::program = nullptr;

///
///
///
Debug::Debug(BulletPhysics::System* system, Engine::Graphics::Camera* camera)
   : mSystem(system)
   , mCamera(camera)
   , mActive(true)
   , mColor(0, 0, 0)
{
}

///
///
///

void Debug::Configure(Engine::EntityManager&, Engine::EventManager&)
{
   if (mSystem && mSystem->GetWorld())
   {
      mSystem->GetWorld()->setDebugDrawer(this);
      setDebugMode(
         btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawAabb
      );
   }

   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load(
          Asset::Shader("BulletDebug.vert"),
          Asset::Shader("BulletDebug.frag")
      );
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading BulletDebug shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);
      program->Uniform("uProjMatrix");
      program->Uniform("uViewMatrix");
      program->Uniform("uModelMatrix");
      program->Uniform("uColor");
      program->Attrib("aPosition");
   }
}

///
///
///
void Debug::Update(Engine::EntityManager&, Engine::EventManager&, TIMEDELTA)
{
   flushLines();
}

///
///
///
void Debug::drawLine(const btVector3& btFrom, const btVector3& btTo, const btVector3& btColor)
{
   if (!mActive)
   {
      return;
   }

   glm::vec3 color{btColor.x(), btColor.y(), btColor.z()};
   if (mColor != color || mPoints.size() >= 512)
   {
      flushLines();
      mColor = color;
   }

   glm::vec3 from{btFrom.x(), btFrom.y(), btFrom.z()};
   glm::vec3 to{btTo.x(), btTo.y(), btTo.z()};
   mPoints.push_back(from);
   mPoints.push_back(to);
}

void Debug::flushLines()
{
   if (mPoints.empty())
   {
      return;
   }

   if (!mActive)
   {
      mPoints.clear();
      return;
   }

   CHECK_GL_ERRORS();
   BIND_PROGRAM_IN_SCOPE(program);
   CHECK_GL_ERRORS();

   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   glm::mat4 model(1);
   program->UniformMatrix4f("uProjMatrix", perspective);
   program->UniformMatrix4f("uViewMatrix", view);
   program->UniformMatrix4f("uModelMatrix", model);
   program->UniformVector3f("uColor", mColor);
   CHECK_GL_ERRORS();

   mVBO.BufferData(sizeof(glm::vec3) * mPoints.size(), mPoints.data(), GL_DYNAMIC_DRAW);
   mVBO.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
   CHECK_GL_ERRORS();

   glDrawArrays(GL_LINES, 0, GLsizei(mPoints.size()));

   CHECK_GL_ERRORS();
   mPoints.clear();
}

void Debug::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
   CUBEWORLD_UNREFERENCED_PARAMETER(PointOnB);
   CUBEWORLD_UNREFERENCED_PARAMETER(normalOnB);
   CUBEWORLD_UNREFERENCED_PARAMETER(distance);
   CUBEWORLD_UNREFERENCED_PARAMETER(lifeTime);
   CUBEWORLD_UNREFERENCED_PARAMETER(color);
}

void Debug::reportErrorWarning(const char* warningString)
{
   LOG_WARNING("Bullet | %1", warningString);
}

void Debug::draw3dText(const btVector3& location, const char* textString)
{
   CUBEWORLD_UNREFERENCED_PARAMETER(location);
   CUBEWORLD_UNREFERENCED_PARAMETER(textString);
}

void Debug::setDebugMode(int mode)
{
   CUBEWORLD_UNREFERENCED_PARAMETER(mode);
}

int Debug::getDebugMode() const
{
   return btIDebugDraw::DBG_DrawWireframe;
}

}; // namespace BulletPhysics

}; // namespace CubeWorld

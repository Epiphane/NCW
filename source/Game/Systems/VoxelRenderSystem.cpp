// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <Engine/Logger/Logger.h>
#include <Engine/Core/Scope.h>
#include <Engine/Graphics/Program.h>

#include <Game/DebugHelper.h>
#include "VoxelRenderSystem.h"

namespace CubeWorld
{

namespace Game
{
   
   VoxelRender::VoxelRender(std::vector<VoxelData>&& voxels)
      : mVoxelData(Engine::Graphics::Vertices)
      , mSize(voxels.size())
   {
      char* data = (char*)&voxels[0];

      LOG_DEBUG("First element: (%1, %2, %3) (%4, %5, %6) %7"\
         , *(float*)data\
         , *(float*)(data + sizeof(float))\
         , *(float*)(data + sizeof(float)*2)\
         , *(float*)(data + sizeof(float)*3)\
         , *(float*)(data + sizeof(float)*4)\
         , *(float*)(data + sizeof(float)*5)\
         , *(uint8_t*)(data + sizeof(float)*6));
      mVoxelData.BufferData(sizeof(VoxelData) * int(voxels.size()), &voxels[0], GL_STATIC_DRAW);
   }
   
   VoxelRender::VoxelRender(const VoxelRender& other)
      : mVoxelData(other.mVoxelData)
   {}

   REGISTER_GLUINT(VoxelRenderSystem, program)
   REGISTER_GLUINT(VoxelRenderSystem, aPosition)
   REGISTER_GLUINT(VoxelRenderSystem, aColor)
   REGISTER_GLUINT(VoxelRenderSystem, aEnabledFaces)
   REGISTER_GLUINT(VoxelRenderSystem, uProjMatrix)
   REGISTER_GLUINT(VoxelRenderSystem, uViewMatrix)
   REGISTER_GLUINT(VoxelRenderSystem, uModelMatrix)
   REGISTER_GLUINT(VoxelRenderSystem, uVoxelSize)

   VoxelRenderSystem::VoxelRenderSystem(Engine::Graphics::Camera* camera) : mCamera(camera)
   {
      Game::DebugHelper::Instance()->RegisterMetric("Voxel Render Time", [this]() -> std::string {
         return Format::FormatString("%1", mClock.Average());
      });
   }

   VoxelRenderSystem::~VoxelRenderSystem()
   {
   }

   void VoxelRenderSystem::Configure(Engine::EntityManager& /*entities*/)
   {
      if (program != 0)
      {
         return;
      }

      program = Engine::Graphics::LoadProgram("Shaders/Voxel.vert", "Shaders/Voxel.geom", "Shaders/Voxel.frag");

      if (program == 0)
      {
         LOG_ERROR("Could not load Voxel shader");
         return;
      }

      DISCOVER_ATTRIBUTE(aPosition);
      DISCOVER_ATTRIBUTE(aColor);
      DISCOVER_ATTRIBUTE(aEnabledFaces);
      DISCOVER_UNIFORM(uProjMatrix);
      DISCOVER_UNIFORM(uViewMatrix);
      DISCOVER_UNIFORM(uModelMatrix);
      DISCOVER_UNIFORM(uVoxelSize);
   }

   using Transform = Engine::Transform;

   void VoxelRenderSystem::Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA)
   {
      glUseProgram(program);

      glm::mat4 perspective = mCamera->GetPerspective();
      glm::mat4 view = mCamera->GetView();
      glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, glm::value_ptr(perspective));
      glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, glm::value_ptr(view));

      mClock.Reset();
      entities.Each<Transform, VoxelRender>([&](Engine::Entity /*entity*/, Transform& transform, VoxelRender& render) {
         render.mVoxelData.AttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelData), (void*)0);
         render.mVoxelData.AttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelData), (void*)(sizeof(float) * 3));
         render.mVoxelData.AttribIPointer(aEnabledFaces, 1, GL_UNSIGNED_BYTE, sizeof(VoxelData), (void*)(sizeof(float) * 6));

         glm::mat4 model(1);
         model = glm::translate(model, transform.position);
         model = glm::rotate(model, transform.GetYaw(), glm::vec3(0, 1, 0));
         model = glm::rotate(model, transform.GetPitch(), glm::vec3(1, 0, 0));
         model = glm::rotate(model, transform.GetRoll(), glm::vec3(0, 0, 1));
         glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));
         
         glDrawArrays(GL_POINTS, 0, render.mSize);
      });
      mClock.Elapsed();

      // Cleanup.
      glDisableVertexAttribArray(aPosition);
      glDisableVertexAttribArray(aColor);
      glDisableVertexAttribArray(aEnabledFaces);
      glUseProgram(0);
   }

}; // namespace Game

}; // namespace CubeWorld

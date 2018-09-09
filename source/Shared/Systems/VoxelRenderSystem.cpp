// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <GL/includes.h>
#include <glm/ext.hpp>

#include <Engine/Logger/Logger.h>
#include <Engine/Core/Scope.h>
#include <Engine/Graphics/Program.h>

#include "../Components/CubeModel.h"
#include "../DebugHelper.h"
#include "AnimationSystem.h"
#include "VoxelRenderSystem.h"

namespace CubeWorld
{

namespace Game
{
   
   VoxelRender::VoxelRender(Voxel::Model&& voxels)
      : mVoxelData(Engine::Graphics::VBO::Vertices)
      , mSize(GLsizei(voxels.size()))
   {
      mVoxelData.BufferData(sizeof(Voxel::Data) * int(voxels.size()), &voxels[0], GL_STATIC_DRAW);
   }
   
   VoxelRender::VoxelRender(const VoxelRender& other)
      : mVoxelData(other.mVoxelData)
   {}

   REGISTER_GLUINT(VoxelRenderSystem, program)
   REGISTER_GLUINT(VoxelRenderSystem, aPosition)
   REGISTER_GLUINT(VoxelRenderSystem, aColor)
   REGISTER_GLUINT(VoxelRenderSystem, aEnabledFaces)
   REGISTER_GLUINT(VoxelRenderSystem, uTint)
   REGISTER_GLUINT(VoxelRenderSystem, uProjMatrix)
   REGISTER_GLUINT(VoxelRenderSystem, uViewMatrix)
   REGISTER_GLUINT(VoxelRenderSystem, uModelMatrix)
   REGISTER_GLUINT(VoxelRenderSystem, uVoxelSize)

   VoxelRenderSystem::VoxelRenderSystem(Engine::Graphics::Camera* camera) : mCamera(camera)
   {
      metric = Game::DebugHelper::Instance()->RegisterMetric("Voxel Render Time", [this]() -> std::string {
         return Format::FormatString("%1ms", std::round(mClock.Average() * 100000.0) / 100);
      });
   }

   VoxelRenderSystem::~VoxelRenderSystem()
   {
   }

   void VoxelRenderSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
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
      DISCOVER_UNIFORM(uTint);
      DISCOVER_UNIFORM(uProjMatrix);
      DISCOVER_UNIFORM(uViewMatrix);
      DISCOVER_UNIFORM(uModelMatrix);
      DISCOVER_UNIFORM(uVoxelSize);
   }

   using Transform = Engine::Transform;

   void VoxelRenderSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
   {
      {GLenum error = glGetError();
   assert(error == 0); }

      glUseProgram(program);
      {GLenum error = glGetError();
   assert(error == 0); }

      glm::mat4 perspective = mCamera->GetPerspective();
      glm::mat4 view = mCamera->GetView();
      glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, glm::value_ptr(perspective));
      glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, glm::value_ptr(view));

      mClock.Reset();
      entities.Each<Transform, VoxelRender>([&](Engine::Entity /*entity*/, Transform& transform, VoxelRender& render) {
         render.mVoxelData.AttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)0);
         render.mVoxelData.AttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)(sizeof(float) * 3));
         render.mVoxelData.AttribIPointer(aEnabledFaces, 1, GL_UNSIGNED_BYTE, sizeof(Voxel::Data), (void*)(sizeof(float) * 6));

         glm::mat4 model = transform.GetMatrix();
         glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));
         glUniform3f(uTint, 255.0f, 255.0f, 255.0f);
         
         glDrawArrays(GL_POINTS, 0, render.mSize);

         GLenum error = glGetError();
         assert(error == 0);
      });

      entities.Each<Transform, CubeModel>([&](Engine::Entity /*entity*/, Transform& transform, CubeModel& cubModel) {
         cubModel.mVBO.AttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)0);
         cubModel.mVBO.AttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)(sizeof(float) * 3));
         cubModel.mVBO.AttribIPointer(aEnabledFaces, 1, GL_UNSIGNED_BYTE, sizeof(Voxel::Data), (void*)(sizeof(float) * 6));

         glm::mat4 model = transform.GetMatrix();
         glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));
         glUniform3fv(uTint, 1, glm::value_ptr(cubModel.mTint));

         glDrawArrays(GL_POINTS, 0, GLsizei(cubModel.mNumVoxels));

         GLenum error = glGetError();
         assert(error == 0);
      });

      entities.Each<Transform, AnimatedSkeleton>([&](Engine::Entity /*entity*/, Transform& transform, AnimatedSkeleton& skeleton) {
         glm::mat4 matrix = transform.GetMatrix();

         for (const AnimatedSkeleton::ModelAttachment& model : skeleton.models)
         {
            AnimatedSkeleton::Bone bone = skeleton.bones[model.bone];
            glm::mat4 boneMatrix = matrix * bone.matrix;

            model.model->mVBO.AttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)0);
            model.model->mVBO.AttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)(sizeof(float) * 3));
            model.model->mVBO.AttribIPointer(aEnabledFaces, 1, GL_UNSIGNED_BYTE, sizeof(Voxel::Data), (void*)(sizeof(float) * 6));

            glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(boneMatrix));
            glUniform3fv(uTint, 1, glm::value_ptr(model.tint));

            glDrawArrays(GL_POINTS, 0, GLsizei(model.model->mVoxelData.size()));

            GLenum error = glGetError();
            assert(error == 0);
         }
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

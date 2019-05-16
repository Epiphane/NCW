// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <GL/includes.h>
#include <glm/ext.hpp>

#include <RGBLogger/Logger.h>
#include <RGBDesignPatterns/Scope.h>
#include <Engine/Graphics/Program.h>

#include "../Components/VoxModel.h"
#include "../DebugHelper.h"
#include "AnimationSystem.h"
#include "VoxelRenderSystem.h"

namespace CubeWorld
{

VoxelRender::VoxelRender()
   : mVoxelData(Engine::Graphics::VBO::Vertices)
   , mSize(0)
{}

VoxelRender::VoxelRender(std::vector<Voxel::Data>&& voxels)
   : mVoxelData(Engine::Graphics::VBO::Vertices)
   , mSize(GLsizei(voxels.size()))
{
   Set(std::move(voxels));
}

VoxelRender::VoxelRender(const VoxelRender& other)
   : mVoxelData(other.mVoxelData)
   , mSize(other.mSize)
{}

void VoxelRender::Set(std::vector<Voxel::Data>&& voxels)
{
   mSize = GLsizei(voxels.size());
   mVoxelData.BufferData(sizeof(Voxel::Data) * voxels.size(), voxels.data(), GL_STATIC_DRAW);
}

void VoxelRender::Set(const std::vector<Voxel::Data>& voxels)
{
   mSize = GLsizei(voxels.size());
   mVoxelData.BufferData(sizeof(Voxel::Data) * voxels.size(), (void*)voxels.data(), GL_STATIC_DRAW);
}

std::unique_ptr<Engine::Graphics::Program> VoxelRenderSystem::program = nullptr;

VoxelRenderSystem::VoxelRenderSystem(Engine::Graphics::Camera* camera) : mCamera(camera)
{
   metric = DebugHelper::Instance().RegisterMetric("Voxel Render Time", [this]() -> std::string {
      return Format::FormatString("%.2fms", mClock.Average() * 1000.0);
   });
}

VoxelRenderSystem::~VoxelRenderSystem()
{
}

void VoxelRenderSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/Voxel.vert", "Shaders/Voxel.geom", "Shaders/Voxel.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading Voxel shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);
      program->Attrib("aPosition");
      program->Attrib("aColor");
      program->Attrib("aEnabledFaces");
      program->Uniform("uProjMatrix");
      program->Uniform("uViewMatrix");
      program->Uniform("uModelMatrix");
      program->Uniform("uTint");
   }
}

using Transform = Engine::Transform;

void VoxelRenderSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   BIND_PROGRAM_IN_SCOPE(program);

   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   program->UniformMatrix4f("uProjMatrix", perspective);
   program->UniformMatrix4f("uViewMatrix", view);

   mClock.Reset();
   entities.Each<Transform, VoxelRender>([&](Engine::Entity /*entity*/, Transform& transform, VoxelRender& render) {
      render.mVoxelData.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)0);
      render.mVoxelData.AttribPointer(program->Attrib("aColor"), 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)(sizeof(float) * 3));
      render.mVoxelData.AttribIPointer(program->Attrib("aEnabledFaces"), 1, GL_UNSIGNED_BYTE, sizeof(Voxel::Data), (void*)(sizeof(float) * 6));

      glm::mat4 model = transform.GetMatrix();
      program->UniformMatrix4f("uModelMatrix", model);
      program->UniformVector3f("uTint", glm::vec3(255.0f));
      
      glDrawArrays(GL_POINTS, 0, render.mSize);

      CHECK_GL_ERRORS();
   });

   entities.Each<Transform, VoxModel>([&](Engine::Entity /*entity*/, Transform& transform, VoxModel& voxModel) {
      voxModel.mVBO.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)0);
      voxModel.mVBO.AttribPointer(program->Attrib("aColor"), 3, GL_FLOAT, GL_FALSE, sizeof(Voxel::Data), (void*)(sizeof(float) * 3));
      voxModel.mVBO.AttribIPointer(program->Attrib("aEnabledFaces"), 1, GL_UNSIGNED_BYTE, sizeof(Voxel::Data), (void*)(sizeof(float) * 6));

      glm::mat4 matrix = transform.GetMatrix();

      for (const VoxModel::Part& part : voxModel.mParts)
      {
         if (part.size == 0 || part.hidden)
         {
            continue;
         }

         glm::mat4 partMatrix = matrix * part.transform;
         glm::vec3 noTint = glm::vec3(255.0f);

         program->UniformMatrix4f("uModelMatrix", partMatrix);
         program->UniformVector3f("uTint", part.tintable ? voxModel.mTint : noTint);

         glDrawArrays(GL_POINTS, GLsizei(part.start), GLsizei(part.size));
         CHECK_GL_ERRORS();
      }
   });
   mClock.Elapsed();

   // Cleanup.
   
}

}; // namespace CubeWorld

// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "VoxModel.h"

namespace CubeWorld
{

VoxModel::VoxModel()
   : mTint(glm::vec3(255))
   , mParts{}
   , mPartLookup{}
   , mVBO(Engine::Graphics::VBO::Vertices)
{
}

VoxModel::VoxModel(const std::string& path, glm::vec3 tint)
{
   Load(path);
   mTint = tint;
}

void VoxModel::Load(const std::string& path)
{
   Maybe<Voxel::VoxModel*> maybeModel = Voxel::VoxFormat::Load(path);
   if (!maybeModel)
   {
      LOG_ERROR(maybeModel.Failure().WithContext("Failed loading VOX model at %1", path).GetMessage());
      return;
   }

   Set(maybeModel.Result());
}

void VoxModel::Set(Voxel::VoxModel* data)
{
   mParts = data->parts;
   mPartLookup = data->partLookup;
   mVBO = data->vbo;
}

}; // namespace CubeWorld

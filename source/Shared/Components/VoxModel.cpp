// By Thomas Steinke

#include <RGBLogger/Logger.h>
#include "../Helpers/Asset.h"

#include "VoxModel.h"

namespace CubeWorld
{

VoxModel::VoxModel()
    : mTint(glm::vec3(255))
    , mParts{}
    , mPartLookup{}
{
}

VoxModel::VoxModel(const BindingProperty& data)
{
    Load(Asset::Path(data["path"]));
    mTint = data["tint"].GetVec3();
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
        maybeModel.Failure().WithContext("Failed loading VOX model at {path}", path).Log();
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

// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBBinding/BindingPropertyMeta.h>
#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <RGBText/Format.h>
#include <Engine/Core/Config.h>

#include "../Helpers/Asset.h"
#include "Skeleton.h"


namespace CubeWorld
{

void Skeleton::Transform(glm::mat4& matrix, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
   matrix = glm::translate(matrix, position);
   matrix = glm::rotate(matrix, RADIANS(rotation.y), glm::vec3(0, 1, 0));
   matrix = glm::rotate(matrix, RADIANS(rotation.x), glm::vec3(1, 0, 0));
   matrix = glm::rotate(matrix, RADIANS(rotation.z), glm::vec3(0, 0, 1));
   matrix = glm::scale(matrix, scale);
}

Skeleton::Skeleton()
{}

Skeleton::Skeleton(const std::string& path)
{
   Load(path);
}

Skeleton::Skeleton(const BindingProperty& data)
{
   Load(data);
}

void Skeleton::Reset()
{
   bones.clear();
   original.clear();
   boneLookup.clear();
   stances.clear();
}

void Skeleton::Load(const std::string& path)
{
   Maybe<BindingProperty> data = YAMLSerializer::DeserializeFile(path);
   if (!data)
   {
      LOG_ERROR(data.Failure().WithContext("Failed loading file").GetMessage());
      return;
   }
   Load(*data);
}

void Skeleton::Load(const BindingProperty& data)
{
   Reset();

   deserialize(*this, data);

   if (defaultModel.empty())
   {
      LOG_ERROR("No default model provided");
      return;
   }

   // Load VOX model for bone data.
   Maybe<Voxel::VoxModel*> maybeModel = Voxel::VoxFormat::Load(Asset::Model(defaultModel));
   if (!maybeModel)
   {
      LOG_ERROR(maybeModel.Failure().WithContext("Failed loading reference model").GetMessage());
      return;
   }

   // Load bones
   Voxel::VoxModel* voxModel = *maybeModel;
   original.resize(voxModel->parts.size());
   for (const Voxel::VoxModel::Part& part : voxModel->parts)
   {
      Bone& bone = original[part.id];
      bone.name = name + "." + part.name;
      bone.parent = name + "." + voxModel->parts[voxModel->parents[part.id]].name;
      bone.position = part.position;
      bone.rotation = part.rotation;
      bone.scale = glm::vec3(1);

      boneLookup.emplace(bone.name, part.id);
   }

   bones.assign(original.begin(), original.end());
}

BindingProperty Skeleton::Serialize()
{
   return serialize(*this);
}

}; // namespace CubeWorld

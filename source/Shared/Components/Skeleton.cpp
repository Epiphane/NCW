// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
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

Skeleton::Skeleton(const BindingProperty& data)
{
   Load(data);
}

Skeleton::Skeleton(Engine::ComponentHandle<VoxModel> model)
   : model(model)
{}

Skeleton::Skeleton(const BindingProperty& data, Engine::ComponentHandle<VoxModel> model)
   : model(model)
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

void Skeleton::Load(const BindingProperty& data)
{
   Reset();

   name = data["name"];
   parent = data["parent"];
   defaultModel = data["default_model"];

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

   Voxel::VoxModel* voxModel = *maybeModel;
   if (model)
   {
      model->Set(voxModel);
   }

   // Load bones
   original.resize(voxModel->parts.size());
   for (const Voxel::VoxModel::Part& part : voxModel->parts)
   {
      Bone& bone = original[part.id];
      bone.name = part.name;
      bone.parent = voxModel->parts[voxModel->parents[part.id]].name;
      bone.position = part.position;
      bone.rotation = part.rotation;
      bone.scale = glm::vec3(1);

      boneLookup.emplace(bone.name, part.id);
   }

   bones.assign(original.begin(), original.end());

   // Load stances
   for (const auto& [id, def] : data["stances"].pairs())
   {
      Stance& stance = stances[id];
      stance.name = id;
      stance.parent = def["inherit"].GetStringValue("base");
      for (const auto& [boneName, boneDef] : def["bones"].pairs())
      {
         if (boneDef["position"].IsVec3())
         {
            stance.positions[boneName] = boneDef["position"].GetVec3();
         }
         if (boneDef["rotation"].IsVec3())
         {
            stance.rotations[boneName] = boneDef["rotation"].GetVec3();
         }
         if (boneDef["scale"].IsVec3())
         {
            stance.scales[boneName] = boneDef["scale"].GetVec3();
         }
         if (boneDef["parent"].IsString())
         {
            stance.parents[boneName] = boneDef["parent"];
         }
      }
   }

   // Make sure we have a base stance!
   Stance& base = stances["base"];
   base.name = "base";
   base.parent = "";
}

BindingProperty Skeleton::Serialize()
{
   BindingProperty result;

   return result;
}

}; // namespace CubeWorld

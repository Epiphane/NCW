// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
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

   // Load stances
   if (data["stances"][0]["name"] != "base")
   {
      Stance base;
      base.name = "base";
      base.parent = "";
      
      stanceLookup.emplace("base", 0);
      stances.push_back(std::move(base));
   }

   for (const auto& def : data["stances"])
   {
      Stance stance;
      stance.name = def["name"];
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

      stanceLookup.emplace(stance.name, stances.size());
      stances.push_back(std::move(stance));
   }
}

BindingProperty Skeleton::Serialize()
{
   BindingProperty result;

   result["name"] = name;
   result["parent"] = parent;
   result["default_model"] = defaultModel;

   for (const auto& stance : stances)
   {
      BindingProperty def;
      def["name"] = stance.name;
      def["inherit"] = stance.parent;
      BindingProperty& bones = def["bones"];
      for (const auto&[bone, pos] : stance.positions)
      {
         bones[bone]["position"] = pos;
      }
      for (const auto&[bone, rot] : stance.rotations)
      {
         bones[bone]["rotation"] = rot;
      }
      for (const auto&[bone, scl] : stance.scales)
      {
         bones[bone]["scale"] = scl;
      }
      for (const auto&[bone, par] : stance.parents)
      {
         bones[bone]["parent"] = par;
      }

      result["stances"].push_back(std::move(def));
   }

   return result;
}

}; // namespace CubeWorld

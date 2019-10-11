// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <Engine/Core/Config.h>
#include <Engine/Core/FileSystemProvider.h>

#include "../Helpers/Asset.h"
#include "SkeletonAnimations.h"


namespace CubeWorld
{

SkeletonAnimations::SkeletonAnimations()
{}

SkeletonAnimations::SkeletonAnimations(const std::string& entity)
{
   Load(entity);
}

SkeletonAnimations::SkeletonAnimations(const std::string& entity, const BindingProperty& data)
{
   Load(entity, data);
}

void SkeletonAnimations::Reset()
{
   states.clear();
}

void SkeletonAnimations::Load(const std::string& entity_)
{
   Reset();

   BindingProperty data(BindingProperty::kObjectType);

   FileSystem& fs = Engine::FileSystemProvider::Instance();
   std::string dir = Asset::Animation(entity_);
   Maybe<std::vector<FileSystem::FileEntry>> maybeFiles = fs.ListDirectory(dir, false, false);
   if (!maybeFiles)
   {
      maybeFiles.Failure().WithContext("Failed loading animations for entity {entity}", entity_).Log();
      return;
   }

   for (const FileSystem::FileEntry& entry : *maybeFiles)
   {
      Maybe<BindingProperty> animation = YAMLSerializer::DeserializeFile(fs, Paths::Join(dir, entry.name));
      if (!animation)
      {
         animation.Failure().WithContext("Failed loading animation {name}", entry.name).Log();
         continue;
      }

      data.Set((*animation)["name"], std::move(*animation));
   }

   Load(entity_, data);
}

void SkeletonAnimations::Load(const std::string& entity_, const BindingProperty& data)
{
   Reset();
   entity = entity_;

   Binding::deserialize(states, data);
}

BindingProperty SkeletonAnimations::Serialize()
{
   return Binding::serialize(*this);
}

}; // namespace CubeWorld

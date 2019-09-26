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

   deserialize(states, data);
}

BindingProperty SkeletonAnimations::Serialize()
{
   BindingProperty result;

   for (const auto&[name, state] : states)
   {
      BindingProperty& data = result[name];
      data["name"] = name;
      data["next"] = state.next;
      data["stance"] = state.stance;
      data["length"] = state.length;

      for (const Keyframe& keyframe : state.keyframes)
      {
         BindingProperty keyframeData;

         keyframeData["time"] = keyframe.time;

         for (const auto&[bone, pos] : keyframe.positions)
         {
            keyframeData["bones"][bone]["position"] = pos;
         }
         for (const auto&[bone, rot] : keyframe.rotations)
         {
            keyframeData["bones"][bone]["rotation"] = rot;
         }
         for (const auto&[bone, scl] : keyframe.scales)
         {
            keyframeData["bones"][bone]["scale"] = scl;
         }

         data["keyframes"].push_back(std::move(keyframeData));
      }

      for (const Transition& transition : state.transitions)
      {
         BindingProperty transitionData;

         transitionData["to"] = transition.destination;
         transitionData["time"] = transition.time;

         for (const Transition::Trigger& trigger : transition.triggers)
         {
            BindingProperty triggerData;

            triggerData["parameter"] = trigger.parameter;
            switch (trigger.type)
            {
            case Transition::Trigger::LessThan:
               triggerData["lt"] = trigger.doubleVal;
               break;
            case Transition::Trigger::GreaterThan:
               triggerData["gte"] = trigger.doubleVal;
               break;
            case Transition::Trigger::Bool:
               triggerData["bool"] = trigger.boolVal;
               break;
            }

            transitionData["triggers"].push_back(std::move(triggerData));
         }

         data["transitions"].push_back(std::move(transitionData));
      }

      for (const ParticleEffect& effect : state.particles)
      {
         BindingProperty effectData;
         effectData["name"] = effect.name;
         effectData["bone"] = effect.bone;
         effectData["start"] = effect.start;
         effectData["end"] = effect.end;
         effectData["mods"] = effect.modifications;
         data["particles"].push_back(std::move(effectData));
      }

      for (const Event& evt : state.events)
      {
         BindingProperty eventData;
         switch (evt.type)
         {
         case Event::Type::Strike:
            eventData["type"] = "strike";
            break;
         default:
            eventData["type"] = "unknown";
            break;
         }

         eventData["start"] = evt.start;
         eventData["end"] = evt.end;
         eventData["properties"] = evt.properties;
         data["events"].push_back(std::move(eventData));
      }
   }

   return result;
}

}; // namespace CubeWorld

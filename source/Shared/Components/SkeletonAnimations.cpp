// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/JSONSerializer.h>
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
   transitions.clear();
}

void SkeletonAnimations::Load(const std::string& entity)
{
   Reset();

   BindingProperty data(BindingProperty::kArrayType);

   FileSystem* fs = Engine::FileSystemProvider::Instance();
   std::string dir = Asset::Animation(entity);
   Maybe<std::vector<FileSystem::FileEntry>> maybeFiles = fs->ListDirectory(dir, false, false);
   if (!maybeFiles)
   {
      LOG_ERROR("Failed loading animations for entity %1: %2", entity, maybeFiles.Failure().GetMessage());
      return;
   }

   for (const FileSystem::FileEntry& entry : *maybeFiles)
   {
      Maybe<BindingProperty> animation = JSONSerializer::DeserializeFile(*fs, Paths::Join(dir, entry.name));
      if (!animation)
      {
         LOG_ERROR("Failed loading animation %1: %2", entry.name, animation.Failure().GetMessage());
         continue;
      }

      data.push_back(std::move(*animation));
   }

   Load(entity, data);
}

void SkeletonAnimations::Load(const std::string& entity, const BindingProperty& data)
{
   Reset();
   this->entity = entity;

   for (const BindingProperty& anim : data)
   {
      std::string name = anim["name"];
      State& state = states[name];
      state.name = name;
      state.next = anim["next"];
      state.stance = anim["stance"].GetStringValue("base");
      state.length = anim["length"].GetDoubleValue();

      assert(state.name != "");
      assert(state.length > 0);

      double lastTime = -1;
      for (const auto& frame : anim["keyframes"])
      {
         Keyframe keyframe;
         keyframe.time = frame["time"].GetDoubleValue();

         assert(keyframe.time > lastTime);
         lastTime = keyframe.time;

         for (const auto&[bone, modification] : frame["bones"].pairs())
         {
            if (const auto& pos = modification["position"]; pos.IsVec3())
            {
               keyframe.positions.emplace(bone, pos.GetVec3());
            }
            if (const auto& rot = modification["rotation"]; rot.IsVec3())
            {
               keyframe.rotations.emplace(bone, rot.GetVec3());
            }
            if (const auto& scl = modification["scale"]; scl.IsVec3())
            {
               keyframe.scales.emplace(bone, scl.GetVec3());
            }
         }
         state.keyframes.push_back(std::move(keyframe));
      }

      std::vector<Transition>& transitionData = transitions[state.name];

      for (const BindingProperty& info : anim["transitions"])
      {
         Transition transition;
         transition.destination = info["to"];
         transition.time = info["time"].GetDoubleValue();
         for (const BindingProperty& triggerInfo : info["triggers"])
         {
            Transition::Trigger trigger;
            trigger.parameter = triggerInfo["parameter"];
            if (const auto& gte = triggerInfo["gte"]; gte.IsDouble())
            {
               trigger.type = Transition::Trigger::GreaterThan;
               trigger.doubleVal = gte.GetDoubleValue();
            }
            if (const auto& lt = triggerInfo["lt"]; lt.IsDouble())
            {
               trigger.type = Transition::Trigger::LessThan;
               trigger.doubleVal = lt.GetDoubleValue();
            }
            if (const auto& boolean = triggerInfo["bool"]; boolean.IsBool())
            {
               trigger.type = Transition::Trigger::Bool;
               trigger.boolVal = boolean.GetBooleanValue();
            }

            transition.triggers.push_back(std::move(trigger));
         }

         transitionData.push_back(std::move(transition));
      }
   }
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

      for (const Transition& transition : transitions[state.name])
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
   }

   return result;
}

}; // namespace CubeWorld

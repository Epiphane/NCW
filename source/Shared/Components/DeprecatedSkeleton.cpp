// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/JSONSerializer.h>
#include <Engine/Core/Config.h>
#include <Engine/Core/FileSystemProvider.h>

#include "../Helpers/Asset.h"
#include "../Helpers/VoxFormat.h"
#include "../Event/NamedEvent.h"
#include "DeprecatedSkeleton.h"


namespace CubeWorld
{

namespace
{

void Transform(glm::mat4& matrix, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
   matrix = glm::translate(matrix, position);
   matrix = glm::rotate(matrix, RADIANS(rotation.y), glm::vec3(0, 1, 0));
   matrix = glm::rotate(matrix, RADIANS(rotation.x), glm::vec3(1, 0, 0));
   matrix = glm::rotate(matrix, RADIANS(rotation.z), glm::vec3(0, 0, 1));
   matrix = glm::scale(matrix, scale);
}

}; // anonymous namespace

void DeprecatedSkeleton::ComputeBoneMatrix(size_t boneId)
{
   Bone& bone = bones[boneId];
   if (boneId == 0)
   {
      bone.matrix = glm::mat4(1);
   }
   else
   {
      // TODO: This may assert one day if stances get super funky.
      // Until thennnnn let's hope they don't :)
      // tl;dr is that a few things are going on:
      // 1. VoxModel is a list of parts
      // 2. Those parts correspond 1:1 with a list of bones
      // 3. Assigning bones to new parents (in a non-base stance) means
      //    that bone N might now have parent M, when M > N
      // 4. These matrices are computed in order, so a violation of
      //    the child > parent order means that a child bone would
      //    be computed BEFORE it's parent.
      // It could be as easy as "if boneId > bone.parent, compute
      // bone parent first", but I don't want to overengineer so
      // we can cross that bridge when we come to it.
      assert(boneId > bone.parent);
      bone.matrix = bones[bone.parent].matrix;
   }

   Transform(bone.matrix, bone.position, bone.rotation, bone.scale);
}

DeprecatedSkeleton::DeprecatedSkeleton()
{
   Reset();
}

void DeprecatedSkeleton::Reset()
{
   states.clear();
   statesByName.clear();
   transitions.clear();
   bones.clear();
   bonesByName.clear();
}

void DeprecatedSkeleton::Load(const std::string& filename)
{
   Reset();

   Maybe<BindingProperty> maybeData = JSONSerializer::DeserializeFile(filename + ".json");
   if (!maybeData)
   {
      LOG_ERROR("Failed reading %1: %2", filename, maybeData.Failure().GetMessage());
      return;
   }

   BindingProperty data = std::move(maybeData.Result());

   name = data["name"].GetStringValue(Paths::GetRelativePath(filename, Asset::Skeleton("")));
   if (auto it = name.rfind('.'); it != std::string::npos)
   {
      name.erase(it);
   }

   parent = data["parent"];
   if (auto it = parent.rfind('.'); it != std::string::npos)
   {
      parent.erase(it);
   }

   modelFilename = data["model"];
   if (modelFilename.empty())
   {
      LOG_ERROR("No model provided");
      return;
   }

   // Load VOX model for bone data.
   Maybe<Voxel::VoxModel*> maybeModel = Voxel::VoxFormat::Load(Asset::Model(modelFilename));
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

   bones.resize(voxModel->parts.size());
   for (const Voxel::VoxModel::Part& part : voxModel->parts)
   {
      DeprecatedSkeleton::Bone& bone = bones[part.id];
      bone.name = part.name;
      bone.position = bone.originalPosition = part.position;
      bone.rotation = bone.originalRotation = part.rotation;
      bone.scale = bone.originalScale = glm::vec3(1);
      bone.parent = voxModel->parents[part.id];
      bone.children.clear();
      ComputeBoneMatrix(part.id);

      if (bone.parent != part.id)
      {
         bones[bone.parent].children.push_back(part.id);
      }

      if (bone.name != "")
      {
         bonesByName.emplace(bone.name, part.id);
      }
   }

   // Create base stance
   /*if (data["stances"][0]["name"] != "base")
   {
      Stance stance;
      stance.name = "base";
      stance.inherit = "base";
      stance.bones = bones;
      stances.push_back(std::move(stance));
      stancesByName.emplace("base", 0);
   }*/

   for (const auto& def : data["stances"])
   {
      Stance stance;
      stance.name = def["name"];
      stance.inherit = def["inherit"].GetStringValue("base");

      if (stance.name.empty())
      {
         LOG_ERROR("Empty stance name. Skipping");
         continue;
      }

      if (stance.name == "base")
      {
         stance.bones = bones;
      }
      else if (stancesByName.find(stance.inherit) == stancesByName.end())
      {
         LOG_ERROR("Stance '%1' specifies inherit from '%2', which does not exist", stance.name, stance.inherit);
         continue;
      }
      else
      {
         stance.parentBone = stances[stancesByName[stance.inherit]].parentBone;
         stance.bones = stances[stancesByName[stance.inherit]].bones;
      }

      for (Bone& bone : stance.bones)
      {
         bone.children.clear();
      }

      // Apply modifications
      for (const auto&[boneName, boneData] : def["bones"].pairs())
      {
         auto boneId = bonesByName.find(boneName);
         if (boneId == bonesByName.end())
         {
            LOG_WARNING("Skipping bone %1, because it couldn't be found in the model", boneName);
            continue;
         }

         DeprecatedSkeleton::Bone& bone = stance.bones[boneId->second];
         if (std::string parentBoneName = boneData["parent"]; !parentBoneName.empty())
         {
            if (boneId->second == 0)
            {
               stance.parentBone = parentBoneName;
            }
            else if (bonesByName.find(parentBoneName) == bonesByName.end())
            {
               LOG_WARNING("Bone '%1' specifies a parent of '%2', which doesn't exist. Nice try kiddo", bone.name, parentBoneName);
            }
            else
            {
               bone.parent = bonesByName[parentBoneName];
               stance.bones[bone.parent].children.push_back(boneId->second);
            }
         }

         bone.position = bone.originalPosition = boneData["position"].GetVec3(bone.originalPosition);
         bone.rotation = bone.originalRotation = boneData["rotation"].GetVec3(bone.originalRotation);
         bone.scale = bone.originalScale = boneData["scale"].GetVec3(bone.originalScale);
      }

      stancesByName.emplace(stance.name, stances.size());
      stances.push_back(std::move(stance));
   }

   // Find and add animation states.
   Maybe<std::vector<FileSystem::FileEntry>> animations = DiskFileSystem{}.ListDirectory(
      Asset::Animation(name),
      false, /* includeDirectories */
      true /* recursive */
   );
   if (!animations)
   {
      LOG_ERROR("Failed loading animations in %1: %2", Asset::Animation(name), animations.Failure().GetMessage());
   }
   else
   {
      for (const FileSystem::FileEntry& entry : *animations)
      {
         Maybe<BindingProperty> maybeAnimation = JSONSerializer::DeserializeFile(Paths::Join(Asset::Animation(name), entry.name));
         if (!maybeAnimation)
         {
            LOG_ERROR("Failed reading %1: %2", entry.name, maybeAnimation.Failure().GetMessage());
            continue;
         }

         const BindingProperty anim = std::move(*maybeAnimation);

         DeprecatedSkeleton::State state;
         state.name = anim["name"];
         state.next = anim["next"];
         state.stance = anim["stance"].GetStringValue("base");
         state.length = anim["length"].GetDoubleValue(1);

         assert(state.name != "");
         assert(state.length > 0);

         double lastTime = -1;
         for (const auto& frame : anim["keyframes"])
         {
            DeprecatedSkeleton::Keyframe keyframe;
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

         statesByName.emplace(state.name, states.size());
         states.push_back(std::move(state));
      }
   }

   // Add transitions.
   for (const auto& [src, definitions] : data["transitions"].pairs())
   {
      std::vector<Transition>& source = transitions[src];

      for (const BindingProperty& info : definitions)
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
               trigger.type = Transition::Trigger::FloatGte;
               trigger.doubleVal = gte.GetDoubleValue();
            }
            if (const auto& lt = triggerInfo["gte"]; lt.IsDouble())
            {
               trigger.type = Transition::Trigger::FloatLt;
               trigger.doubleVal = lt.GetDoubleValue();
            }
            if (const auto& boolean = triggerInfo["bool"]; boolean.IsBool())
            {
               trigger.type = Transition::Trigger::Bool;
               trigger.boolVal = boolean.GetBooleanValue();
            }

            transition.triggers.push_back(std::move(trigger));
         }
         source.push_back(std::move(transition));
      }
   }
}

std::string DeprecatedSkeleton::Serialize()
{
   BindingProperty data;
   data["version"] = 1;
   data["model"] = modelFilename;
   data["parent"] = parent;

   // Bones
   for (const DeprecatedSkeleton::Bone& bone : bones)
   {
      BindingProperty info;
      info["parent"] = bones[bone.parent].name;
      info["position"] = bone.originalPosition;
      info["rotation"] = bone.originalRotation;
      info["scale"] = bone.originalScale;
      data["bones"][bone.name] = info;
   }

   if (!parent.empty())
   {
      data["bones"]["root"]["parent"] = parentBone;
   }

   // States and their transitions
   if (states.size() > 0)
   {
      data["default"] = states[0].name;
      for (const State& state : states)
      {
         BindingProperty stateData;
         stateData["name"] = state.name;
         stateData["next"] = state.next;
         stateData["length"] = std::round(state.length * 100) / 100;

         for (const Keyframe& keyframe : state.keyframes)
         {
            if (keyframe.time == state.length)
            {
               continue;
            }

            BindingProperty keyframeData;

            keyframeData["time"] = std::round(keyframe.time * 100) / 100;

            for (const auto& modification : keyframe.positions)
            {
               keyframeData["bones"][modification.first]["position"] = modification.second;
            }
            for (const auto& modification : keyframe.rotations)
            {
               keyframeData["bones"][modification.first]["rotation"] = modification.second;
            }
            for (const auto& modification : keyframe.scales)
            {
               keyframeData["bones"][modification.first]["scale"] = modification.second;
            }

            stateData["keyframes"].push_back(keyframeData);
         }

         data["states"].push_back(stateData);
      }
   }

   // Add transitions for this state.
   for (const auto& [state, list] : transitions)
   {
      for (const Transition& transition : list)
      {
         BindingProperty transitionData;

         transitionData["to"] = transition.destination;
         transitionData["time"] = std::round(transition.time * 100) / 100;
         for (auto trigger : transition.triggers)
         {
            BindingProperty triggerData;

            triggerData["parameter"] = trigger.parameter;
            switch (trigger.type)
            {
            case DeprecatedSkeleton::Transition::Trigger::FloatGte:
               triggerData["gte"] = std::round(trigger.doubleVal * 100) / 100;
               break;
            case DeprecatedSkeleton::Transition::Trigger::FloatLt:
               triggerData["lt"] = std::round(trigger.doubleVal * 100) / 100;
               break;
            case DeprecatedSkeleton::Transition::Trigger::Bool:
               triggerData["bool"] = trigger.boolVal;
               break;
            default:
               assert(false && "Unrecognized trigger type");
            }

            transitionData["triggers"].push_back(triggerData);
         }

         data["transitions"][state].push_back(transitionData);
      }
   }

   auto result = JSONSerializer::Serialize(data);
   if (!result)
   {
      LOG_ERROR("Failed to serialize skeleton: %1", result.Failure().GetMessage());
   }
   return *result;
}

}; // namespace CubeWorld

// By Thomas Steinke

#include <algorithm>
#include <fstream>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <stack>
#include <Engine/Logger/Logger.h>
#include <Engine/Core/Config.h>
#include <Engine/Core/Paths.h>

#include "../Helpers/JsonHelper.h"
#include "../Helpers/VoxFormat.h"
#include "../Event/NamedEvent.h"
#include "AnimatedSkeleton.h"

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

void AnimatedSkeleton::ComputeBoneMatrix(size_t boneId)
{
   Bone& bone = bones[boneId];
   if (boneId == 0)
   {
      bone.matrix = glm::mat4(1);
   }
   else
   {
      bone.matrix = bones[bone.parent].matrix;
   }

   Transform(bone.matrix, bone.position, bone.rotation, bone.scale);
}

AnimatedSkeleton::AnimatedSkeleton()
{
   Reset();
}

void AnimatedSkeleton::Reset()
{
   states.clear();
   statesByName.clear();
   bones.clear();
   bonesByName.clear();
}

void AnimatedSkeleton::Load(const std::string& filename)
{
   std::ifstream file(filename);
   nlohmann::json data;
   file >> data;

   //name = Paths::GetFilename(filename);
   //Load(Paths::GetDirectory(filename), data);
//}

//void AnimatedSkeleton::Load(const std::string& workingDirectory, const nlohmann::json& data)
//{
   Reset();

   name = Paths::GetFilename(filename);
   std::string workingDirectory = Paths::GetDirectory(filename);

   // Load VOX model for bone data.
   parentFilename = data.value("parent", "");

   modelFilename = data.value("model", "");
   if (modelFilename == "")
   {
      LOG_ERROR("No model provided");
      return;
   }
   else
   {
      Maybe<Voxel::VoxModel*> maybeModel = Voxel::VoxFormat::Load(Paths::Join(workingDirectory, modelFilename));
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
         AnimatedSkeleton::Bone& bone = bones[part.id];
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

      // Examine the file's skeleton for consistency, and then apply it.
      for (auto it = data["bones"].begin(); it != data["bones"].end(); it++)
      {
         std::string boneName = it.key();

         auto boneId = bonesByName.find(boneName);
         if (boneId == bonesByName.end())
         {
            LOG_WARNING("Skipping bone %1, because it couldn't be found in the model", boneName);
            continue;
         }

         AnimatedSkeleton::Bone& bone = bones[boneId->second];
         AnimatedSkeleton::Bone& parent = bones[bone.parent];
         const nlohmann::json& boneData = it.value();
         if (boneId->second > 0 && boneData["parent"] != parent.name)
         {
            continue;
         }
         
         if (boneId->second == 0)
         {
            parentBone = boneData.value("parent", "root");
         }

         bone.position = bone.originalPosition = Shared::JsonHelpers::JsonToVec3(boneData["position"]);
         bone.rotation = bone.originalRotation = Shared::JsonHelpers::JsonToVec3(boneData["rotation"]);
         bone.scale = bone.originalScale = Shared::JsonHelpers::JsonToVec3(boneData["scale"]);
      }
   }

   // Add animation states.
   for (auto anim : data["states"])
   {
      AnimatedSkeleton::State state;
      state.name = anim.value("name", "");
      state.length = anim["length"];

      assert(state.name != "");
      assert(state.length > 0);

      double lastTime = -1;
      for (auto frame : anim["keyframes"])
      {
         AnimatedSkeleton::Keyframe keyframe;
         keyframe.time = frame["time"];

         assert(keyframe.time > lastTime);
         lastTime = keyframe.time;

         if (auto boneData = frame.find("bones"); boneData != frame.end())
         {
            for (auto modification = boneData.value().begin(); modification != boneData.value().end(); modification++)
            {
               std::string boneName = modification.key();

               if (auto pos = modification.value().find("position"); pos != modification.value().end())
               {
                  keyframe.positions.emplace(boneName, Shared::JsonHelpers::JsonToVec3(pos.value()));
               }
               if (auto rot = modification.value().find("rotation"); rot != modification.value().end())
               {
                  keyframe.rotations.emplace(boneName, Shared::JsonHelpers::JsonToVec3(rot.value()));
               }
               if (auto scl = modification.value().find("scale"); scl != modification.value().end())
               {
                  keyframe.scales.emplace(boneName, Shared::JsonHelpers::JsonToVec3(scl.value()));
               }
            }
         }
         state.keyframes.push_back(std::move(keyframe));
      }

      statesByName.emplace(state.name, states.size());
      states.push_back(std::move(state));
   }

   //assert(states.size() > 0);

   // Add transitions.
   if (data.find("transitions") != data.end())
   {
      for (auto it = data["transitions"].begin(); it != data["transitions"].end(); it++)
      {
         State& source = states[statesByName[it.key()]];

         for (auto info : it.value())
         {
            Transition transition;
            transition.destination = info.value("to", "");
            transition.time = info.value("time", 0.0);
            for (auto triggerInfo : info["triggers"])
            {
               Transition::Trigger trigger;
               trigger.parameter = triggerInfo.value("parameter", "");
               if (auto gte = triggerInfo.find("gte"); gte != triggerInfo.end())
               {
                  trigger.type = Transition::Trigger::FloatGte;
                  trigger.floatVal = gte.value();
               }
               if (auto lt = triggerInfo.find("lt"); lt != triggerInfo.end())
               {
                  trigger.type = Transition::Trigger::FloatLt;
                  trigger.floatVal = lt.value();
               }
               if (auto boolean = triggerInfo.find("bool"); boolean != triggerInfo.end())
               {
                  trigger.type = Transition::Trigger::Bool;
                  trigger.boolVal = boolean.value();
               }

               transition.triggers.push_back(std::move(trigger));
            }
            source.transitions.push_back(std::move(transition));
         }
      }
   }
}

std::string AnimatedSkeleton::Serialize()
{
   nlohmann::json data;
   data["version"] = 1;
   data["model"] = modelFilename;
   data["parent"] = parentFilename;

   // Bones
   for (const AnimatedSkeleton::Bone& bone : bones)
   {
      nlohmann::json info;
      info["parent"] = bones[bone.parent].name;
      info["position"] = Shared::JsonHelpers::Vec3ToJson(bone.originalPosition);
      info["rotation"] = Shared::JsonHelpers::Vec3ToJson(bone.originalRotation);
      info["scale"] = Shared::JsonHelpers::Vec3ToJson(bone.originalScale);
      data["bones"][bone.name] = info;
   }

   if (!parentFilename.empty())
   {
      data["bones"]["root"]["parent"] = parentBone;
   }

   // States and their transitions
   data["default"] = states[0].name;
   for (auto state : states)
   {
      nlohmann::json stateData;
      stateData["name"] = state.name;
      stateData["length"] = std::round(state.length * 100) / 100;

      for (auto keyframe : state.keyframes)
      {
         if (keyframe.time == state.length)
         {
            continue;
         }

         nlohmann::json keyframeData;

         keyframeData["time"] = std::round(keyframe.time * 100) / 100;

         for (const auto& modification : keyframe.positions)
         {
            keyframeData["bones"][modification.first]["position"] = Shared::JsonHelpers::Vec3ToJson(modification.second);
         }
         for (const auto& modification : keyframe.rotations)
         {
            keyframeData["bones"][modification.first]["rotation"] = Shared::JsonHelpers::Vec3ToJson(modification.second);
         }
         for (const auto& modification : keyframe.scales)
         {
            keyframeData["bones"][modification.first]["scale"] = Shared::JsonHelpers::Vec3ToJson(modification.second);
         }

         stateData["keyframes"].push_back(keyframeData);
      }

      data["states"].push_back(stateData);

      // Add transitions for this state.
      for (auto transition : state.transitions)
      {
         nlohmann::json transitionData;

         transitionData["to"] = transition.destination;
         transitionData["time"] = std::round(transition.time * 100) / 100;
         for (auto trigger : transition.triggers)
         {
            nlohmann::json triggerData;

            triggerData["parameter"] = trigger.parameter;
            switch (trigger.type)
            {
               case AnimatedSkeleton::Transition::Trigger::FloatGte:
                  triggerData["gte"] = (double)std::round(trigger.floatVal * 100) / 100;
                  break;
               case AnimatedSkeleton::Transition::Trigger::FloatLt:
                  triggerData["lt"] = (double)std::round(trigger.floatVal * 100) / 100;
                  break;
               case AnimatedSkeleton::Transition::Trigger::Bool:
                  triggerData["bool"] = trigger.boolVal;
                  break;
               default:
                  assert(false && "Unrecognized trigger type");
            }

            transitionData["triggers"].push_back(triggerData);
         }
      
         data["transitions"][state.name].push_back(transitionData);
      }
   }

   return data.dump(3);
}

}; // namespace CubeWorld

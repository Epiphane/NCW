// By Thomas Steinke

#include <fstream>
#include <glm/ext.hpp>
#include <stack>
#include <Engine/Logger/Logger.h>
#include <Engine/Core/Config.h>

#include "../Helpers/json.hpp"
#include "../Event/NamedEvent.h"
#include "AnimationSystem.h"

namespace CubeWorld
{

namespace Game
{

namespace
{

glm::vec3 JsonToVec3(const nlohmann::json& json)
{
   return glm::vec3(json[0], json[1], json[2]);
}

size_t AddBoneToSkeleton(AnimatedSkeleton* skeleton, const size_t parent, const std::string& name, const nlohmann::json& data)
{
   AnimatedSkeleton::Bone bone;
   bone.name = name;
   bone.position = JsonToVec3(data["position"]);
   bone.rotation = JsonToVec3(data["rotation"]);
   bone.parent = parent;

   size_t id = skeleton->bones.size();
   skeleton->bones.push_back(std::move(bone));
   skeleton->bonesByName.emplace(name, id);
   skeleton->ComputeBoneMatrix(id);

   if (auto bones = data.find("bones"); bones != data.end())
   {
      for (auto it = bones->begin(); it != bones->end(); it++)
      {
         // Real talk: this was originally one line:
         //   children.push_back(AddBoneToSkeleton(...));
         // But this segfaults on Mac. Hypothesis is that skeleton->bones[id].children
         // gets evaluated before AddBoneToSkeleton(...). As part of that function, 
         // skeleton->bones changes, which technically invalidates all existing references
         // including the previously-mentioned piece here. If the vector then gets reallocated
         // skeleton->bones[id] won't change, but will reference garbage memory.
         // There are two solutions
         //   1. Do what has been done. Obviously I chose this one, referencing bones[id] only
         //      after its state is guaranteed not to change
         //   2. Evaluate a reference to skeleton->bones[id].children and then do the original
         //      addition. That would be a lot more confusing IMO.
         size_t child = AddBoneToSkeleton(skeleton, id, it.key(), it.value());
         skeleton->bones[id].children.push_back(child);
      }
   }

   return id;
}

nlohmann::json Vec3ToJson(const glm::vec3& vec3)
{
   return {vec3.x, vec3.y, vec3.z};
}

nlohmann::json SerializeBone(AnimatedSkeleton* skeleton, AnimatedSkeleton::Bone& bone)
{
   nlohmann::json data;

   data["position"] = Vec3ToJson(bone.position);
   data["rotation"] = Vec3ToJson(bone.rotation);
   
   for (auto childId : bone.children)
   {
      AnimatedSkeleton::Bone& child = skeleton->bones[childId];
      data["bones"][child.name] = SerializeBone(skeleton, child);
   }

   return data;
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

   bone.matrix = glm::translate(bone.matrix, bone.position);
   bone.matrix = glm::rotate(bone.matrix, RADIANS(bone.rotation.y), glm::vec3(0, 1, 0));
   bone.matrix = glm::rotate(bone.matrix, RADIANS(bone.rotation.x), glm::vec3(1, 0, 0));
   bone.matrix = glm::rotate(bone.matrix, RADIANS(bone.rotation.z), glm::vec3(0, 0, 1));
}

AnimatedSkeleton::AnimatedSkeleton(const std::string& filename)
{
   Load(filename);
}

void AnimatedSkeleton::Load(const std::string& filename)
{
   states.clear();
   statesByName.clear();
   bones.clear();
   bonesByName.clear();
   floatParams.clear();
   boolParams.clear();
   models.clear();
   current = 0;
   time = 0;
   next = 0;
   transitionCurrent = 0;
   transitionStart = 0;
   transitionEnd = 0;

   std::ifstream file(filename);
   nlohmann::json data;
   file >> data;

   // Add bone data.
   for (auto it = data["bones"].begin(); it != data["bones"].end(); it++)
   {
      AddBoneToSkeleton(this, 0, it.key(), it.value());
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

         // By default, every bone is in its original spot.
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.positions), [](const Bone& b) { return b.position; });
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.rotations), [](const Bone& b) { return b.rotation; });
         std::transform(bones.begin(), bones.end(), std::back_inserter(keyframe.matrixes), [](const Bone& b) { return b.matrix; });

         if (auto boneData = frame.find("bones"); boneData != frame.end())
         {
            for (size_t boneId = 0; boneId < bones.size(); boneId++)
            {
               Bone original = bones[boneId];

               glm::vec3 position = original.position;
               glm::vec3 rotation = original.rotation;
               if (auto modification = boneData.value().find(original.name); modification != boneData.value().end())
               {
                  if (auto pos = modification.value().find("position"); pos != modification.value().end())
                  {
                     position = keyframe.positions[boneId] = JsonToVec3(pos.value());
                  }
                  if (auto rot = modification.value().find("rotation"); rot != modification.value().end())
                  {
                     rotation = keyframe.rotations[boneId] = JsonToVec3(rot.value());
                  }
               }

               if (boneId == 0)
               {
                  keyframe.matrixes[boneId] = glm::mat4(1);
               }
               else
               {
                  keyframe.matrixes[boneId] = keyframe.matrixes[original.parent];
               }

               keyframe.matrixes[boneId] = glm::translate(keyframe.matrixes[boneId], position);
               keyframe.matrixes[boneId] = glm::rotate(keyframe.matrixes[boneId], RADIANS(rotation.y), glm::vec3(0, 1, 0));
               keyframe.matrixes[boneId] = glm::rotate(keyframe.matrixes[boneId], RADIANS(rotation.x), glm::vec3(1, 0, 0));
               keyframe.matrixes[boneId] = glm::rotate(keyframe.matrixes[boneId], RADIANS(rotation.z), glm::vec3(0, 0, 1));
            }
         }
         state.keyframes.push_back(std::move(keyframe));
      }

      Keyframe end = state.keyframes[0];
      end.time = state.length;
      state.keyframes.push_back(end);

      statesByName.emplace(state.name, states.size());
      states.push_back(std::move(state));
   }

   assert(states.size() > 0);

   // Add transitions.
   if (data.find("transitions") != data.end())
   {
      for (auto it = data["transitions"].begin(); it != data["transitions"].end(); it++)
      {
         State& source = states[statesByName[it.key()]];

         for (auto info : it.value())
         {
            Transition transition;
            transition.destination = info["to"];
            transition.time = info.value("time", 0.0);
            for (auto triggerInfo : info["triggers"])
            {
               Transition::Trigger trigger;
               trigger.parameter = triggerInfo["parameter"];
               if (auto gte = triggerInfo.find("gte"); gte != triggerInfo.end())
               {
                  floatParams[trigger.parameter] = 0;
                  trigger.type = Transition::Trigger::FloatGte;
                  trigger.floatVal = gte.value();
               }
               if (auto lt = triggerInfo.find("lt"); lt != triggerInfo.end())
               {
                  floatParams[trigger.parameter] = 0;
                  trigger.type = Transition::Trigger::FloatLt;
                  trigger.floatVal = lt.value();
               }
               if (auto boolean = triggerInfo.find("bool"); boolean != triggerInfo.end())
               {
                  boolParams[trigger.parameter] = 0;
                  trigger.type = Transition::Trigger::Bool;
                  trigger.boolVal = boolean.value();
               }

               transition.triggers.push_back(std::move(trigger));
            }
            source.transitions.push_back(std::move(transition));
         }
      }
   }

   if (data.find("default") != data.end())
   {
      Play(data["default"]);
   }
   else
   {
      Play(states[0].name);
   }

   next = 0;
   transitionCurrent = 0;
   transitionStart = 0;
   transitionEnd = 0;
}

std::string AnimatedSkeleton::Serialize()
{
   nlohmann::json data;
   data["version"] = 1;

   // Bones
   data["bones"][bones[0].name] = SerializeBone(this, bones[0]);
   data["default"] = states[0].name;

   // States and their transitions
   for (auto state : states)
   {
      nlohmann::json stateData;
      stateData["name"] = state.name;
      stateData["length"] = state.length;

      for (auto keyframe : state.keyframes)
      {
         if (keyframe.time == state.length)
         {
            continue;
         }

         nlohmann::json keyframeData;

         keyframeData["time"] = keyframe.time;

         for (size_t boneId = 0; boneId < bones.size(); boneId++)
         {
            Bone& bone = bones[boneId];
            if (keyframe.positions[boneId] != bone.position)
            {
               keyframeData["bones"][bone.name]["position"] = Vec3ToJson(keyframe.positions[boneId]);
            }
            if (keyframe.rotations[boneId] != bone.rotation)
            {
               keyframeData["bones"][bone.name]["rotation"] = Vec3ToJson(keyframe.rotations[boneId]);
            }
         }

         stateData["keyframes"].push_back(keyframeData);
      }

      data["states"].push_back(stateData);

      // Add transitions for this state.
      for (auto transition : state.transitions)
      {
         nlohmann::json transitionData;

         transitionData["to"] = transition.destination;
         transitionData["time"] = transition.time;
         for (auto trigger : transition.triggers)
         {
            nlohmann::json triggerData;

            triggerData["parameter"] = trigger.parameter;
            switch (trigger.type)
            {
               case AnimatedSkeleton::Transition::Trigger::FloatGte:
                  triggerData["gte"] = trigger.floatVal;
                  break;
               case AnimatedSkeleton::Transition::Trigger::FloatLt:
                  triggerData["lt"] = trigger.floatVal;
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

void AnimatedSkeleton::AddModel(const BoneWeights& bones, const std::string& model)
{
   assert(bones.size() == 1 && "Multiple bones not supported");
   ModelAttachment attachment;
   assert(bonesByName.find(bones[0].first) != bonesByName.end());
   attachment.bone = bonesByName.find(bones[0].first)->second;
   attachment.weight = 1;
   attachment.model = CubeModelInfo::Load(model, false);
   assert(attachment.model != nullptr);
   attachment.tint = glm::vec3(255);

   models.push_back(std::move(attachment));
}

void AnimatedSkeleton::AddModel(const BoneWeights& bones, const std::string& model, glm::vec3 tint)
{
   assert(bones.size() == 1 && "Multiple bones not supported");
   ModelAttachment attachment;
   assert(bonesByName.find(bones[0].first) != bonesByName.end());
   attachment.bone = bonesByName.find(bones[0].first)->second;
   attachment.weight = 1;
   attachment.model = CubeModelInfo::Load(model, true);
   assert(attachment.model != nullptr);
   attachment.tint = tint;

   models.push_back(std::move(attachment));
}

void AnimatedSkeleton::Play(const std::string& state, double startTime)
{
   auto it = statesByName.find(state);
   assert(it != statesByName.end());

   if (current == it->second)
   {
      return;
   }

   current = next = it->second;
   time = startTime;
}

void AnimatedSkeleton::TransitionTo(const std::string& state, double transitionTime, double startTime)
{
   // If a transition is in flight, skip to the end of it.
   if (current != next)
   { 
      current = next;
      time = transitionCurrent;
   }

   auto it = statesByName.find(state);
   assert(it != statesByName.end());

   if (current == it->second && next == it->second)
   {
      return;
   }

   next = it->second;
   transitionStart = startTime;
   transitionCurrent = startTime;
   transitionEnd = startTime + transitionTime;
}

void AnimationSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   // First, update skeletons.
   entities.Each<AnimatedSkeleton>([&](Engine::Entity /*entity*/, AnimatedSkeleton& skeleton) {
      // Advance basic animation
      {
         AnimatedSkeleton::State& state = skeleton.states[skeleton.current];
         skeleton.time += dt;
         while (skeleton.time >= state.length)
         {
            skeleton.time -= state.length;
         }

         size_t keyframeIndex = state.keyframes.size() - 1;
         while (skeleton.time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
         {
            keyframeIndex--;
         }

         const AnimatedSkeleton::Keyframe& src = state.keyframes[keyframeIndex];
         const AnimatedSkeleton::Keyframe& dst = state.keyframes[keyframeIndex + 1];
         const float progress = (skeleton.time - src.time) / (dst.time - src.time);

         for (size_t boneId = 0; boneId < skeleton.bones.size(); ++boneId)
         {
            skeleton.bones[boneId].matrix = progress * dst.matrixes[boneId] + (1 - progress) * src.matrixes[boneId];
         }
      }

      // Transitions!
      if (skeleton.current != skeleton.next)
      {
         AnimatedSkeleton::State& state = skeleton.states[skeleton.next];
         skeleton.transitionCurrent = skeleton.transitionCurrent + dt;
         float transitionProgress;
         if (skeleton.transitionCurrent < skeleton.transitionEnd)
         {
            transitionProgress = skeleton.transitionCurrent / (skeleton.transitionEnd - skeleton.transitionStart);
         }
         else
         {
            transitionProgress = 1;
            skeleton.current = skeleton.next;
            skeleton.time = skeleton.transitionCurrent;
         }

         float time = skeleton.transitionCurrent;
         while (time >= state.length)
         {
            time -= state.length;
         }

         size_t keyframeIndex = state.keyframes.size() - 1;
         while (time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
         {
            keyframeIndex--;
         }

         const AnimatedSkeleton::Keyframe& src = state.keyframes[keyframeIndex];
         const AnimatedSkeleton::Keyframe& dst = state.keyframes[keyframeIndex + 1];
         const float progress = (time - src.time) / (dst.time - src.time);

         for (size_t boneId = 0; boneId < skeleton.bones.size(); ++boneId)
         {
            glm::mat4 matrix = progress * dst.matrixes[boneId] + (1 - progress) * src.matrixes[boneId];
            skeleton.bones[boneId].matrix = transitionProgress * matrix + (1 - transitionProgress) * skeleton.bones[boneId].matrix;
         }
      }

      // Compute new transitions
      if (skeleton.current == skeleton.next) {
         AnimatedSkeleton::State& state = skeleton.states[skeleton.current];
         for (AnimatedSkeleton::Transition& transition : state.transitions)
         {
            // Check triggers.
            bool valid = true;
            for (AnimatedSkeleton::Transition::Trigger& trigger : transition.triggers)
            {
               switch (trigger.type)
               {
               case AnimatedSkeleton::Transition::Trigger::FloatGte:
                  valid &= skeleton.floatParams[trigger.parameter] >= trigger.floatVal;
                  break;
               case AnimatedSkeleton::Transition::Trigger::FloatLt:
                  valid &= skeleton.floatParams[trigger.parameter] < trigger.floatVal;
                  break;
               case AnimatedSkeleton::Transition::Trigger::Bool:
                  valid &= skeleton.boolParams[trigger.parameter] == trigger.boolVal;
                  break;
               default:
                  assert(false && "Unrecognized trigger type");
               }
            }

            if (valid)
            {
               skeleton.TransitionTo(transition.destination, transition.time);
               break;
            }
         }
      }
   });
}

}; // namespace Game

}; // namespace CubeWorld

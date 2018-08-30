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

         std::vector<glm::vec3> positions;
         std::vector<glm::vec3> rotations;
         std::transform(bones.begin(), bones.end(), std::back_inserter(positions), [](const Bone& b) { return b.position; });
         std::transform(bones.begin(), bones.end(), std::back_inserter(rotations), [](const Bone& b) { return b.rotation; });

         // By default, every bone is in its original spot.
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
                     position = JsonToVec3(pos.value());
                  }
                  if (auto rot = modification.value().find("rotation"); rot != modification.value().end())
                  {
                     rotation = JsonToVec3(rot.value());
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
   });
}
   
}; // namespace Game

}; // namespace CubeWorld

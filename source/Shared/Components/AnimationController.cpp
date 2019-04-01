// By Thomas Steinke

#include <fstream>

#include <Engine/Core/Paths.h>
#include <Engine/Logger/Logger.h>
#include <Engine/Entity/EntityManager.h>

#include "AnimationController.h"

namespace CubeWorld
{

AnimationController::AnimationController()
   : skeletons{}
   , skeletonRootId{}
   , skeletonParents{}
   , bonesByName{}
   , numBones{0}
   , states{}
   , floatParams{}
   , boolParams{}
   , current{0}
   , time{0}
   , next{0}
   , transitionCurrent{0}
   , transitionStart{0}
   , transitionEnd{0}
{}

void AnimationController::Reset()
{
   skeletons.clear();
   skeletonRootId.clear();
   skeletonParents.clear();
   bonesByName.clear();
   numBones = 0;
   states.clear();
   statesByName.clear();
   floatParams.clear();
   boolParams.clear();
   current = 0;
   time = 0;
   next = 0;
   transitionCurrent = 0;
   transitionStart = 0;
   transitionEnd = 0;
}

AnimationController::State& AnimationController::GetCurrentState()
{
   return states[current];
}

void AnimationController::AddSkeleton(Engine::ComponentHandle<AnimatedSkeleton> skeleton)
{
   // Could optimize this later
   std::pair<size_t, size_t> parent = {0, 0};
   for (size_t i = 0; i < skeletons.size(); i++)
   {
      if (skeletons[i]->name == Paths::GetFilename(skeleton->parentFilename))
      {
         parent.first = i;
         
         auto bone = skeletons[i]->bonesByName.find(skeleton->parentBone);
         if (bone == skeletons[i]->bonesByName.end())
         {
            LOG_ERROR("Could not find bone '%1' in skeleton '%2'", skeletons[i]->name, skeleton->parentBone);
         }
         else {
            parent.second = bone->second;
         }
         break;
      }
   }

   size_t rootId = numBones;
   
   skeletons.push_back(skeleton);
   skeletonRootId.push_back(rootId);
   skeletonParents.push_back(parent);

   for (const auto& bone : skeleton->bones)
   {
      // If there's a collision, leave the first bone with that name (the parent-most)
      const auto& existing = bonesByName.find(bone.name);
      if (existing == bonesByName.end())
      {
         bonesByName.emplace(bone.name, numBones);
      }
      numBones++;
   }

   for (State& state : states)
   {
      for (Keyframe& keyframe : state.keyframes)
      {
         std::transform(skeleton->bones.begin(), skeleton->bones.end(), std::back_inserter(keyframe.positions), [](const AnimatedSkeleton::Bone& bone) { return bone.originalPosition; });
         std::transform(skeleton->bones.begin(), skeleton->bones.end(), std::back_inserter(keyframe.rotations), [](const AnimatedSkeleton::Bone& bone) { return bone.originalRotation; });
         std::transform(skeleton->bones.begin(), skeleton->bones.end(), std::back_inserter(keyframe.scales), [](const AnimatedSkeleton::Bone& bone) { return bone.originalScale; });
      }
   }

   // Modify or add states
   for (const AnimatedSkeleton::State& newState : skeleton->states)
   {
      size_t stateId = states.size();
      auto existing = statesByName.find(newState.name);
      if (existing != statesByName.end())
      {
         stateId = existing->second;
      }
      else
      {
         // Construct the new state
         statesByName.emplace(newState.name, stateId);

         State state;
         state.name = newState.name;
         state.length = newState.length;
         state.keyframes.resize(newState.keyframes.size());

         for (size_t i = 0; i < newState.keyframes.size(); i++)
         {
            Keyframe& keyframe = state.keyframes[i];
            keyframe.time = newState.keyframes[i].time;

            for (const auto& s : skeletons)
            {
               std::transform(s->bones.begin(), s->bones.end(), std::back_inserter(keyframe.positions), [](const AnimatedSkeleton::Bone& bone) { return bone.originalPosition; });
               std::transform(s->bones.begin(), s->bones.end(), std::back_inserter(keyframe.rotations), [](const AnimatedSkeleton::Bone& bone) { return bone.originalRotation; });
               std::transform(s->bones.begin(), s->bones.end(), std::back_inserter(keyframe.scales), [](const AnimatedSkeleton::Bone& bone) { return bone.originalScale; });
            }
         }

         states.push_back(std::move(state));
      }

      // Add this skeleton's info to the state
      State& state = states[stateId];

      if (newState.length != 0 && newState.length != state.length)
      {
         LOG_ERROR("New state data's length (%1) does not match actual state length (%2). Skipping...", newState.length, state.length);
         continue;
      }

      size_t j = 0;
      for (const AnimatedSkeleton::Keyframe& newKeyframe : newState.keyframes)
      {
         // Find the existing keyframe at this time or after
         while (j < state.keyframes.size() && state.keyframes[j].time < newKeyframe.time)
         {
            j++;
         }

         if (j >= state.keyframes.size() || state.keyframes[j].time > newKeyframe.time)
         {
            LOG_ERROR("No available keyframe at time %1 in the parent animation. Skipping data...");
         }
         else // if (keyframe.time == newKeyframe.time)
         {
            // Found a match! Apply modifications
            Keyframe& keyframe = state.keyframes[j];
            for (const auto&[bone, pos] : newKeyframe.positions)
            {
               if (skeleton->bonesByName.find(bone) != skeleton->bonesByName.end())
               {
                  keyframe.positions[rootId + skeleton->bonesByName[bone]] = pos;
               }
               else
               {
                  keyframe.positions[bonesByName[bone]] = pos;
               }
            }
            for (const auto&[bone, rot] : newKeyframe.rotations)
            {
               if (skeleton->bonesByName.find(bone) != skeleton->bonesByName.end())
               {
                  keyframe.rotations[rootId + skeleton->bonesByName[bone]] = rot;
               }
               else
               {
                  keyframe.rotations[bonesByName[bone]] = rot;
               }
            }
            for (const auto&[bone, scl] : newKeyframe.scales)
            {
               if (skeleton->bonesByName.find(bone) != skeleton->bonesByName.end())
               {
                  keyframe.scales[rootId + skeleton->bonesByName[bone]] = scl;
               }
               else
               {
                  keyframe.scales[bonesByName[bone]] = scl;
               }
            }
         }
      }
   }
}

Engine::ComponentHandle<AnimatedSkeleton> AnimationController::GetSkeletonForBone(BoneID id)
{
   for (auto& skeleton : skeletons)
   {
      if (skeleton->bones.size() > id)
      {
         return skeleton;
      }
      id -= skeleton->bones.size();
   }
   return {};
}

AnimatedSkeleton::Bone* AnimationController::GetBone(BoneID id)
{
   for (auto& skeleton : skeletons)
   {
      if (skeleton->bones.size() > id)
      {
         return &skeleton->bones[id];
      }
      id -= skeleton->bones.size();
   }
   return nullptr;
}

AnimationController::BoneID AnimationController::NextBone(BoneID id)
{
   return (id < numBones) ? id + 1 : 0;
}

AnimationController::BoneID AnimationController::PrevBone(BoneID id)
{
   return (id == 0) ? numBones - 1 : id - 1;
}

AnimationController::BoneID AnimationController::ParentBone(BoneID id)
{
   for (size_t i = 0; i < skeletons.size(); ++i)
   {
      const auto& skeleton = skeletons[i];
      if (skeleton->bones.size() > id)
      {
         if (id == 0)
         {
            const auto& parent = skeletonParents[i];
            return skeletonRootId[parent.first] + parent.second;
         }
         else
         {
            return skeletonRootId[i] + skeleton->bones[id].parent;
         }
      }
      id -= skeleton->bones.size();
   }
   return 0;
}

void AnimationController::Play(const std::string& state, double startTime)
{
   // TODO this and transitions lol
   auto it = skeletons[0]->statesByName.find(state);
   assert(it != skeletons[0]->statesByName.end());

   if (current == it->second)
   {
      return;
   }

   current = next = it->second;
   time = startTime;
}

void AnimationController::TransitionTo(const std::string& state, double transitionTime, double startTime)
{
   // If a transition is in flight, skip to the end of it.
   if (current != next)
   { 
      current = next;
      time = transitionCurrent;
   }

   auto it = skeletons[0]->statesByName.find(state);
   assert(it != skeletons[0]->statesByName.end());

   if (current == it->second && next == it->second)
   {
      return;
   }

   next = it->second;
   transitionStart = startTime;
   transitionCurrent = startTime;
   transitionEnd = startTime + transitionTime;
}

}; // namespace CubeWorld

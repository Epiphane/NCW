// By Thomas Steinke

#include <algorithm>
#include <deque>
#include <fstream>

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <Engine/Entity/EntityManager.h>

#include "AnimationController.h"

namespace CubeWorld
{

AnimationController::AnimationController()
{
   Reset();
}

void AnimationController::Reset()
{
   skeletons.clear();
   skeletonRootId.clear();
   bones.clear();
   bonesByName.clear();
   states.clear();
   statesByName.clear();
   stances.clear();
   floatParams.clear();
   boolParams.clear();
   current = 0;
   time = 0;
   next = 0;
   transitionCurrent = 0;
   transitionStart = 0;
   transitionEnd = 0;
}

void AnimationController::UpdateSkeletonStates()
{
   for (Engine::ComponentHandle<AnimatedSkeleton>& skeleton : skeletons)
   {
      skeleton->states.clear();
      skeleton->statesByName.clear();
      skeleton->transitions.clear();
   }

   std::vector<AnimatedSkeleton::State> stateData;

   // For looking up later
   std::vector<glm::vec3> originalPosition;
   std::vector<glm::vec3> originalRotation;
   std::vector<glm::vec3> originalScale;
   for (Engine::ComponentHandle<AnimatedSkeleton>& skeleton : skeletons)
   {
      for (const AnimatedSkeleton::Bone& bone : skeleton->bones)
      {
         originalPosition.push_back(bone.originalPosition);
         originalRotation.push_back(bone.originalRotation);
         originalScale.push_back(bone.originalScale);
      }
   }

   for (const State& state : states)
   {
      AnimatedSkeleton::State info;
      info.name = state.name;
      info.next = state.next;
      info.length = state.length;

      for (const Keyframe& keyframe : state.keyframes)
      {
         info.keyframes.push_back(AnimatedSkeleton::Keyframe{keyframe.time});
      }

      // Start at the skeleton that _created_ the state. This way we don't add
      // state details for a sub-skeleton's state to it's parent, thus changing
      // the ownership.
      for (size_t s = state.skeletonId; s < skeletons.size(); ++s)
      {
         Engine::ComponentHandle<AnimatedSkeleton>& skeleton = skeletons[s];
         for (AnimatedSkeleton::Keyframe& keyframeInfo : info.keyframes)
         {
            keyframeInfo.positions.clear();
            keyframeInfo.rotations.clear();
            keyframeInfo.scales.clear();
         }

         // Case 1: This is the skeleton creating the state - consider all bones.
         // Case 2: This skeleton is extending the state - consider all of this skeleton's bones.
         size_t first = state.skeletonId == s ? 0 : skeletonRootId[s];
         size_t last = s == skeletons.size() - 1 ? bones.size() : skeletonRootId[s + 1];
         
         bool modified = (s == state.skeletonId);
         for (size_t b = first; b < last; ++b)
         {
            std::string name = bones[b];
            for (size_t i = 0; i < state.keyframes.size(); ++i)
            {
               const Keyframe& keyframe = state.keyframes[i];
               AnimatedSkeleton::Keyframe& keyframeInfo = info.keyframes[i];

               if (keyframe.positions[b] != originalPosition[b])
               {
                  keyframeInfo.positions[name] = keyframe.positions[b];
                  modified = true;
               }
               if (keyframe.rotations[b] != originalRotation[b])
               {
                  keyframeInfo.rotations[name] = keyframe.rotations[b];
                  modified = true;
               }
               if (keyframe.scales[b] != originalScale[b])
               {
                  keyframeInfo.scales[name] = keyframe.scales[b];
                  modified = true;
               }
            }
         }

         if (modified)
         {
            skeleton->statesByName.emplace(info.name, skeleton->states.size());
            skeleton->states.push_back(info);
         }
         
         // Now, add transition information
         for (const Transition& transition : state.transitions)
         {
            // Case 1: This is the skeleton creating the state - add if this is the dest skeleton too.
            // Case 2: This skeleton is extending the state - add only if the destination is new.
            const State& other = states[statesByName[transition.destination]];
            if (other.skeletonId == s)
            {
               skeleton->transitions[state.name].push_back(transition);
            }
         }
      }
   }
}

AnimationController::State& AnimationController::GetCurrentState()
{
   return states[current];
}

void AnimationController::AddSkeleton(Engine::ComponentHandle<AnimatedSkeleton> skeleton)
{
   // Could optimize this later
   Engine::ComponentHandle<AnimatedSkeleton> parentSkeleton;
   size_t parentRootId = 0;
   for (size_t i = 0; i < skeletons.size(); ++i)
   {
      if (skeletons[i]->name == skeleton->parent)
      {
         parentSkeleton = skeletons[i];
         parentRootId = skeletonRootId[i];
         break;
      }
   }

   size_t rootId = bones.size();
   
   skeletons.push_back(skeleton);
   skeletonRootId.push_back(rootId);

   // Add bones
   for (const auto& bone : skeleton->bones)
   {
      // If there's a collision, leave the first bone with that name (the parent-most)
      const auto& existing = bonesByName.find(bone.name);
      if (existing == bonesByName.end())
      {
         bonesByName.emplace(bone.name, bones.size());
      }
      bones.push_back(bone.name);
   }

   // Add stances
   for (const auto& stance : skeleton->stances)
   {
      auto it = std::find_if(stances.begin(), stances.end(), [&](const Stance& s) { return stance.name == s.name; });
      if (it == stances.end())
      {
         Stance newStance;
         newStance.name = stance.name;
         newStance.inherit = stance.inherit;

         if (stance.name == "base")
         {
            assert(rootId == 0 && "The base stance must be defined by the very first skeleton");
         }
         else
         {
            auto parent = std::find_if(stances.rbegin(), stances.rend(), [&](const Stance& s) { return s.name == stance.inherit; });
            if (parent == stances.rend())
            {
               LOG_ERROR("I don't know what to do, no stance '%1' found (parent of '%2')", stance.inherit, stance.name);
               newStance.parents = stances[0].parents;
            }
            else
            {
               newStance.parents = parent->parents;
            }
         }

         stances.push_back(std::move(newStance));
      }
   }

   // Prepare existing stances
   for (Stance& stance : stances)
   {
      std::string stanceName = stance.name;
      std::vector<AnimatedSkeleton::Stance>::iterator it;
      do
      {
         it = std::find_if(skeleton->stances.begin(), skeleton->stances.end(), [&](const AnimatedSkeleton::Stance& stance) { return stance.name == stanceName; });
         stanceName = GetStance(stanceName).inherit;
      } while (it == skeleton->stances.end());

      std::transform(it->bones.begin(), it->bones.end(), std::back_inserter(stance.parents), [&](const AnimatedSkeleton::Bone& bone) { return rootId + bone.parent; });

      if (rootId > 0)
      {
         size_t b = rootId - 1;
         for (; b > 0; --b)
         {
            if (bones[b] == it->parentBone)
            {
               break;
            }
         }
         stance.parents[rootId] = b;
      }
   }

   for (State& state : states)
   {
      std::string stanceName = stances[state.stance].name;
      std::vector<AnimatedSkeleton::Stance>::iterator stance;
      do
      {
         stance = std::find_if(skeleton->stances.begin(), skeleton->stances.end(), [&](const AnimatedSkeleton::Stance& stance) { return stance.name == stanceName; });
         stanceName = stances[state.stance].inherit;
      } while (stance == skeleton->stances.end() && stanceName != "base");

      for (Keyframe& keyframe : state.keyframes)
      {
         std::transform(stance->bones.begin(), stance->bones.end(), std::back_inserter(keyframe.positions), [](const AnimatedSkeleton::Bone& bone) { return bone.originalPosition; });
         std::transform(stance->bones.begin(), stance->bones.end(), std::back_inserter(keyframe.rotations), [](const AnimatedSkeleton::Bone& bone) { return bone.originalRotation; });
         std::transform(stance->bones.begin(), stance->bones.end(), std::back_inserter(keyframe.scales), [](const AnimatedSkeleton::Bone& bone) { return bone.originalScale; });
      }
   }

   // Modify or add states
   for (const AnimatedSkeleton::State& state : skeleton->states)
   {
      AddState(skeleton, state);
   }

   // Add transitions
   for (const auto&[stateName, transitions] : skeleton->transitions)
   {
      auto stateId = statesByName.find(stateName);
      if (stateId == statesByName.end())
      {
         LOG_ERROR("Tried to add transitions for state '%1', but state does not exist", stateName);
         continue;
      }

      // Append transition data
      State& state = states[stateId->second];
      std::transform(transitions.begin(), transitions.end(), std::back_inserter(state.transitions), [](const auto& t) { return t; });
   }
}

void AnimationController::AddState(Engine::ComponentHandle<AnimatedSkeleton> skeleton, const AnimatedSkeleton::State& definition)
{
   Engine::Entity::ID sID = skeleton.GetEntity().GetID();
   size_t skeletonNdx = 0;
   for (; skeletonNdx < skeletons.size(); ++skeletonNdx)
   {
      if (skeletons[skeletonNdx].GetEntity().GetID() == sID)
      {
         break;
      }
   }

   if (skeletonNdx == skeletons.size())
   {
      LOG_ERROR("Attempt to add a state, using an origin skeleton that doesn't exist.");
      return;
   }

   size_t rootBone = skeletonRootId[skeletonNdx];

   size_t stateId = states.size();
   auto existing = statesByName.find(definition.name);
   if (existing != statesByName.end())
   {
      stateId = existing->second;
   }
   else
   {
      // Construct the new state
      statesByName.emplace(definition.name, stateId);

      State state;
      state.name = definition.name;
      state.next = definition.next;
      state.stance = 0;
      while (stances[state.stance].name != definition.stance)
      {
         ++state.stance;
      }
      state.skeletonId = skeletonNdx;
      state.length = definition.length;
      state.keyframes.resize(definition.keyframes.size());

      for (size_t i = 0; i < definition.keyframes.size(); i++)
      {
         Keyframe& keyframe = state.keyframes[i];
         keyframe.time = definition.keyframes[i].time;

         for (const auto& s : skeletons)
         {
            std::string stanceName = definition.stance;
            std::vector<AnimatedSkeleton::Stance>::const_iterator it;
            do
            {
               it = std::find_if(s->stances.begin(), s->stances.end(), [&](const AnimatedSkeleton::Stance& stance) { return stance.name == stanceName; });
               stanceName = GetStance(stanceName).inherit;
            } while (it == s->stances.end());

            std::transform(it->bones.begin(), it->bones.end(), std::back_inserter(keyframe.positions), [](const AnimatedSkeleton::Bone& bone) { return bone.originalPosition; });
            std::transform(it->bones.begin(), it->bones.end(), std::back_inserter(keyframe.rotations), [](const AnimatedSkeleton::Bone& bone) { return bone.originalRotation; });
            std::transform(it->bones.begin(), it->bones.end(), std::back_inserter(keyframe.scales), [](const AnimatedSkeleton::Bone& bone) { return bone.originalScale; });
         }
      }

      states.push_back(std::move(state));
   }

   // Add this skeleton's info to the state
   State& state = states[stateId];

   if (definition.length != 0 && definition.length != state.length)
   {
      LOG_ERROR("New state data's length (%1) does not match actual state length (%2). Skipping...", definition.length, state.length);
      return;
   }

   size_t j = 0;
   for (const AnimatedSkeleton::Keyframe& newKeyframe : definition.keyframes)
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
               keyframe.positions[rootBone + skeleton->bonesByName[bone]] = pos;
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
               keyframe.rotations[rootBone + skeleton->bonesByName[bone]] = rot;
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
               keyframe.scales[rootBone + skeleton->bonesByName[bone]] = scl;
            }
            else
            {
               keyframe.scales[bonesByName[bone]] = scl;
            }
         }
      }
   }
}

AnimationController::Stance& AnimationController::GetStance(const std::string& name)
{
   const auto it = std::find_if(stances.rbegin(), stances.rend(), [&](const Stance& s) { return s.name == name; });
   assert(it != stances.rend() && "Stance does not exist");
   return *it;
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
   return (id < bones.size()) ? id + 1 : 0;
}

AnimationController::BoneID AnimationController::PrevBone(BoneID id)
{
   return (id == 0) ? bones.size() - 1 : id - 1;
}

AnimationController::BoneID AnimationController::ParentBone(BoneID id)
{
   return stances[states[current].stance].parents[id];
}

void AnimationController::Play(const std::string& state, double startTime)
{
   // TODO this and transitions lol
   auto it = statesByName.find(state);
   assert(it != statesByName.end());

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

}; // namespace CubeWorld

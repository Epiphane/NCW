// By Thomas Steinke

#include <algorithm>
#include <deque>

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <Shared/Helpers/Asset.h>

#include "AnimationController.h"

namespace CubeWorld
{


// For initializing and loading data
AnimationController::AnimationController()
{
   Reset();
}

AnimationController::AnimationController(Engine::ComponentHandle<MultipleParticleEmitters> emitters)
   : emitters(emitters)
{
   Reset();
}

void AnimationController::Reset()
{
   skeletons.clear();
   skeletonRootId.clear();
   bones.clear();
   boneLookup.clear();
   states.clear();
   stateLookup.clear();
   stances.clear();
   current = 0;
   time = 0;
   next = 0;
   lastBasePosition = {0, 0, 0};
   lastTransitionPosition = {0, 0, 0};
   transitionCurrent = 0;
   transitionStart = 0;
   transitionEnd = 0;

   if (emitters)
   {
      emitters->systems.clear();
   }
}

void AnimationController::AddSkeleton(Engine::ComponentHandle<Skeleton> skeleton)
{
   // Could optimize this later
   Engine::ComponentHandle<Skeleton> parentSkeleton;
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
      if (boneLookup.count(bone.name) == 0)
      {
         boneLookup.emplace(bone.name, bones.size());
      }
      bones.push_back(bone.name);
   }

   // Add stances
   for (const Skeleton::Stance& stance : skeleton->stances)
   {
      auto it = std::find_if(stances.begin(), stances.end(), [&](const Stance& s) { return stance.name == s.name; });
      if (it == stances.end())
      {
         Stance newStance;
         newStance.name = stance.name;
         newStance.parent = stance.parent;

         if (stance.name == "base")
         {
            assert(rootId == 0 && "The base stance must be defined by the very first skeleton");
         }
         else
         {
            auto parent = std::find_if(stances.rbegin(), stances.rend(), [&](const Stance& s) { return s.name == stance.parent; });
            assert(parent != stances.rend());
            newStance.parents = parent->parents;
            newStance.positions = parent->positions;
            newStance.rotations = parent->rotations;
            newStance.scales = parent->scales;
         }

         stances.push_back(std::move(newStance));
      }
   }

   // Prepare existing stances
   for (Stance& stance : stances)
   {
      if (stance.name == "base")
      {
         for (const Skeleton::Bone& original : skeleton->original)
         {
            stance.parents.push_back(boneLookup.at(original.parent));
            stance.positions.push_back(original.position);
            stance.rotations.push_back(original.rotation);
            stance.scales.push_back(original.scale);
         }
      }
      else
      {
         std::vector<Stance>::iterator parent = std::find_if(stances.begin(), stances.end(), [&](const Stance& s) { return s.name == stance.parent; });

         stance.parents.insert(stance.parents.end(), parent->parents.begin() + (int64_t)rootId, parent->parents.end());
         stance.positions.insert(stance.positions.end(), parent->positions.begin() + (int64_t)rootId, parent->positions.end());
         stance.rotations.insert(stance.rotations.end(), parent->rotations.begin() + (int64_t)rootId, parent->rotations.end());
         stance.scales.insert(stance.scales.end(), parent->scales.begin() + (int64_t)rootId, parent->scales.end());
      }

      if (skeleton->stanceLookup.count(stance.name) == 0)
      {
         continue;
      }

      const Skeleton::Stance& mod = skeleton->stances[skeleton->stanceLookup.at(stance.name)];

      for (const auto&[bone, parent] : mod.parents)
      {
         stance.parents[boneLookup.at(bone)] = boneLookup.at(parent);
      }
      for (const auto&[bone, pos] : mod.positions)
      {
         stance.positions[boneLookup.at(bone)] = pos;
      }
      for (const auto&[bone, rot] : mod.rotations)
      {
         stance.rotations[boneLookup.at(bone)] = rot;
      }
      for (const auto&[bone, scl] : mod.scales)
      {
         stance.scales[boneLookup.at(bone)] = scl;
      }
   }

   // Modify existing states with default bone properties
   for (State& state : states)
   {
      const Stance& stance = stances[state.stance];

      for (Keyframe& keyframe : state.keyframes)
      {
         keyframe.positions.insert(keyframe.positions.end(), stance.positions.begin() + (int64_t)rootId, stance.positions.end());
         keyframe.rotations.insert(keyframe.rotations.end(), stance.rotations.begin() + (int64_t)rootId, stance.rotations.end());
         keyframe.scales.insert(keyframe.scales.end(), stance.scales.begin() + (int64_t)rootId, stance.scales.end());
      }
   }
}

void AnimationController::AddAnimations(Engine::ComponentHandle<SkeletonAnimations> animations)
{
   for (const auto&[name, mods] : animations->states)
   {
      if (stateLookup.count(name) == 0)
      {
         State newState;
         newState.name = name;
         newState.next = mods.next;

         std::string stance = mods.stance;
         const auto stanceIt = std::find_if(stances.begin(), stances.end(), [&](const Stance& s) { return s.name == stance; });
         assert(stanceIt != stances.end());
         newState.stance = size_t(stanceIt - stances.begin());
         newState.length = mods.length;
         newState.keyframes.resize(mods.keyframes.size());

         for (size_t i = 0; i < mods.keyframes.size(); i++)
         {
            Keyframe& keyframe = newState.keyframes[i];
            keyframe.time = mods.keyframes[i].time;
            keyframe.positions.assign(stanceIt->positions.begin(), stanceIt->positions.end());
            keyframe.rotations.assign(stanceIt->rotations.begin(), stanceIt->rotations.end());
            keyframe.scales.assign(stanceIt->scales.begin(), stanceIt->scales.end());
         }

         stateLookup.emplace(name, states.size());
         states.push_back(std::move(newState));
      }

      State& state = states[stateLookup.at(name)];

      for (const SkeletonAnimations::Keyframe& kframe : mods.keyframes)
      {
         const auto keyframeIt = std::find_if(state.keyframes.begin(), state.keyframes.end(), [&](const Keyframe& k) { return k.time == kframe.time; });
         if (keyframeIt == state.keyframes.end())
         {
            LOG_ERROR("Unable to find a keyframe with time {time} in the original skeleton", kframe.time);
            continue;
         }

         for (const auto&[bone, pos] : kframe.positions)
         {
            keyframeIt->positions[boneLookup.at(bone)] = pos;
         }
         for (const auto&[bone, rot] : kframe.rotations)
         {
            keyframeIt->rotations[boneLookup.at(bone)] = rot;
         }
         for (const auto&[bone, scl] : kframe.scales)
         {
            keyframeIt->scales[boneLookup.at(bone)] = scl;
         }
      }
   }

   // Add transitions
   for (const auto&[name, transitions] : animations->transitions)
   {
      State& state = states[stateLookup.at(name)];

      // Append transition data
      state.transitions.insert(state.transitions.end(), transitions.begin(), transitions.end());
   }

   // Add particle effects
   for (const auto& [name, effects] : animations->effects)
   {
      State& state = states[stateLookup.at(name)];

      if (state.name.empty())
      {
         LOG_ERROR("State {name} was not initialized properly, it had no state definition but had animations", state.name);
         assert(false);
      }

      for (const auto& effectDef : effects)
      {
         MultipleParticleEmitters::Emitter effect(
            Asset::Particle(effectDef.name),
            Asset::ParticleShaders(),
            Asset::Image("")
         );

         effect.useEntityTransform = false;
         effect.update = false;
         effect.render = false;
         effect.ApplyConfiguration(Asset::Image(""), effectDef.modifications);
         effect.Reset();

         EmitterRef ref;
         ref.emitter = emitters->systems.size();
         ref.bone = effectDef.bone;
         ref.start = effectDef.start;
         ref.end = effectDef.end;

         emitters->systems.push_back(std::move(effect));
         state.emitters.push_back(std::move(ref));
      }
   }
}

void AnimationController::Play(const std::string& state, double startTime)
{
   auto it = stateLookup.find(state);
   assert(it != stateLookup.end());

   if (current == it->second)
   {
      return;
   }

   current = next = it->second;
   time = startTime;
   lastBasePosition = {0, 0, 0};
}

void AnimationController::TransitionTo(const std::string& state, double transitionTime, double startTime)
{
   // If a transition is in flight, skip to the end of it.
   if (current != next)
   {
      current = next;
      time = transitionCurrent;
      lastBasePosition = lastTransitionPosition;
   }

   auto it = stateLookup.find(state);
   assert(it != stateLookup.end());

   if (current == it->second && next == it->second)
   {
      return;
   }

   next = it->second;
   transitionStart = startTime;
   transitionCurrent = startTime;
   transitionEnd = startTime + transitionTime;
   lastTransitionPosition = {0, 0, 0};
}

}; // namespace CubeWorld

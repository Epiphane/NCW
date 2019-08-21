// By Thomas Steinke

#include <glm/ext.hpp>
#include <RGBLogger/Logger.h>
#include <RGBText/StringHelper.h>
#include <Shared/Helpers/Asset.h>

#include "SimpleAnimationSystem.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

SimpleAnimationController::SimpleAnimationController()
   : SimpleAnimationController(Engine::ComponentHandle<MultipleParticleEmitters>{})
{}

SimpleAnimationController::SimpleAnimationController(Engine::ComponentHandle<MultipleParticleEmitters> emitters)
   : current("")
   , time(0)
   , next("")
   , transitionCurrent(0)
   , transitionStart(0)
   , transitionEnd(0)
   , emitters(emitters)
{
}

void SimpleAnimationController::Reset()
{
   if (emitters)
   {
      emitters->systems.clear();
   }
   skeletons.clear();
   animations.clear();
   stances.clear();
   states.clear();
}

void SimpleAnimationController::UpdateSkeletonStates()
{
   for (const Engine::ComponentHandle<SkeletonAnimations>& anims : animations)
   {
      anims->states.clear();
   }

   for (const auto&[name, state] : states)
   {
      // Start at the skeleton that _created_ the state. This way we don't add
      // state details for a sub-skeleton's state to it's parent, thus changing
      // the ownership.
      bool found = false;
      std::vector<std::string> skipped;
      for (const Engine::ComponentHandle<SkeletonAnimations>& anims : animations)
      {
         if (!found && anims->entity != state.entity)
         {
            skipped.push_back(anims->entity);
            continue;
         }

         bool modified = !found;
         found = true;

         SkeletonAnimations::State newState;
         newState.name = name;
         newState.next = state.next;
         newState.length = state.length;
         newState.stance = state.stance;
         for (const SkeletonAnimations::Keyframe& keyframe : state.keyframes)
         {
            SkeletonAnimations::Keyframe newKeyframe;
            newKeyframe.time = keyframe.time;

            for (const auto&[bone, pos] : keyframe.positions)
            {
               std::vector<std::string> parts = StringHelper::Split(bone, '.');
               if (anims->entity == parts[0] ||
                   std::find(skipped.begin(), skipped.end(), parts[0]) != skipped.end())
               {
                  newKeyframe.positions.emplace(bone, pos);
               }
            }
            for (const auto&[bone, rot] : keyframe.rotations)
            {
               std::vector<std::string> parts = StringHelper::Split(bone, '.');
               if (anims->entity == parts[0] ||
                  std::find(skipped.begin(), skipped.end(), parts[0]) != skipped.end())
               {
                  newKeyframe.rotations.emplace(bone, rot);
               }
            }
            for (const auto&[bone, scl] : keyframe.scales)
            {
               std::vector<std::string> parts = StringHelper::Split(bone, '.');
               if (anims->entity == parts[0] ||
                  std::find(skipped.begin(), skipped.end(), parts[0]) != skipped.end())
               {
                  newKeyframe.scales.emplace(bone, scl);
               }
            }

            newState.keyframes.push_back(std::move(newKeyframe));
         }

         if (modified)
         {
            anims->states.emplace(name, std::move(newState));
         }

         skipped.clear();
      }
   }
}

void SimpleAnimationController::Play(const std::string& state, double startTime)
{
   current = state;
   time = startTime;
}

void SimpleAnimationController::TransitionTo(const std::string& state, double transitionTime, double startTime)
{
   // If a transition is in flight, skip to the end of it.
   if (current != next)
   { 
      current = next;
      time = transitionCurrent;
   }

   next = state;
   transitionStart = startTime;
   transitionCurrent = startTime;
   transitionEnd = startTime + transitionTime;
}

void SimpleAnimationController::AddSkeleton(Engine::ComponentHandle<Skeleton> skeleton)
{
   skeletons.push_back(skeleton);

   const auto addSkeletonName = [&](const Skeleton::Bone& b) { return b; };
   //   Skeleton::Bone result = b;
   //   result.name = skeleton->name + "." + b.name;
   //   return result;
   //};

   for (const Skeleton::Stance& s : skeleton->stances)
   {
      Stance& stance = stances[s.name];

      // Hasn't been constructed yet
      // At the end of this branch, stance.bones must look as follows:
      // [...{existing values, unchanged}..., ...{parent stance for this skeleton}...]
      size_t firstBone = 0;
      if (stance.name.empty())
      {
         stance.name = s.name;
         stance.parent = s.parent;

         if (stance.parent.empty())
         {
            assert(stance.name == "base" && "Only base stance may have no parent");
            std::transform(skeleton->original.begin(), skeleton->original.end(), std::back_inserter(stance.bones), addSkeletonName);
         }
         else
         {
            const Stance& parent = stances[stance.parent];
            stance.bones.assign(parent.bones.begin(), parent.bones.end());
         }
      }
      else
      {
         firstBone = stance.bones.size();
         if (stance.parent.empty())
         {
            assert(stance.name == "base" && "Only base stance may have no parent");
            std::transform(skeleton->original.begin(), skeleton->original.end(), std::back_inserter(stance.bones), addSkeletonName);
         }
         else
         {
            const Stance& parent = stances[stance.parent];
            stance.bones.insert(stance.bones.end(), parent.bones.begin() + int64_t(firstBone), parent.bones.end());
         }
      }

      for (const auto&[bone, pos] : s.positions)
      {
         stance.bones[firstBone + skeleton->boneLookup[bone]].position = pos;
      }
      for (const auto&[bone, rot] : s.rotations)
      {
         stance.bones[firstBone + skeleton->boneLookup[bone]].rotation = rot;
      }
      for (const auto&[bone, scl] : s.scales)
      {
         stance.bones[firstBone + skeleton->boneLookup[bone]].scale = scl;
      }
      for (const auto&[bone, parent] : s.parents)
      {
         stance.bones[firstBone + skeleton->boneLookup[bone]].parent = parent;
      }
   }
}

void SimpleAnimationController::AddAnimations(Engine::ComponentHandle<SkeletonAnimations> anims)
{
   animations.push_back(anims);

   const auto addEntityName = [&](const auto& b) { return b; };

   for (const auto& [name, s] : anims->states)
   {
      State& state = states[name];

      if (state.name.empty())
      {
         state.entity = anims->entity;
         state.name = name;
         state.next = s.next;
         state.length = s.length;
         state.stance = s.stance;
         state.keyframes.assign(s.keyframes.begin(), s.keyframes.end());
      }
      else
      {
         for (const SkeletonAnimations::Keyframe& k : s.keyframes)
         {
            const auto it = std::find_if(state.keyframes.begin(), state.keyframes.end(), [&](const SkeletonAnimations::Keyframe& kf) { return kf.time == k.time; });
            if (it == state.keyframes.end())
            {
               LOG_ERROR("Keyframe at time %1 has no match in the base state", k.time);
            }
            else
            {
               std::transform(k.positions.begin(), k.positions.end(), std::inserter(it->positions, it->positions.end()), addEntityName);
               std::transform(k.rotations.begin(), k.rotations.end(), std::inserter(it->rotations, it->rotations.end()), addEntityName);
               std::transform(k.scales.begin(), k.scales.end(), std::inserter(it->scales, it->scales.end()), addEntityName);
            }
         }
      }
   }

   // Add particle effects
   for (const auto& [name, effects] : anims->effects)
   {
      State& state = states[name];

      if (state.name.empty())
      {
         LOG_ERROR("State %1 was not initialized properly, it had no state definition but had animations");
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

void SimpleAnimationSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   entities.Each<AnimationSystemController>([&](AnimationSystemController& controller) {
      if (controller.paused)
      {
         dt = controller.nextTick;
         controller.nextTick = 0;
      }
      dt *= controller.speed;
   });

   using Keyframe = SkeletonAnimations::Keyframe;
   using State = SimpleAnimationController::State;
   using Stance = SimpleAnimationController::Stance;

   // First, update skeletons.
   entities.Each<SimpleAnimationController>([&](SimpleAnimationController& controller) {
      // Check for an un-loaded skeleton
      if (controller.skeletons.empty())
      {
         return;
      }

      const State& state = controller.states[controller.current];
      const Stance& stance = controller.stances[state.stance];

      // Progress animation
      controller.time += dt;
      while (controller.time > state.length)
      {
         controller.time -= state.length;
      }

      const std::vector<Keyframe>& keyframes = state.keyframes;
      size_t keyframeIndex = keyframes.size() - 1;
      while (controller.time < keyframes[keyframeIndex].time && keyframeIndex > 0)
      {
         keyframeIndex--;
      }

      bool isLastFrame = (keyframeIndex == keyframes.size() - 1);
      const Keyframe& src = keyframes[keyframeIndex];
      const Keyframe& dst = isLastFrame ? keyframes[0] : keyframes[keyframeIndex + 1];
      const double dstTime = isLastFrame ? state.length : dst.time;
      float progress = 0.0f;
      if (dstTime > src.time)
      {
         progress = float(controller.time - src.time) / float(dstTime - src.time);
      }

      size_t boneId = 0;
      for (const auto& skeleton : controller.skeletons)
      {
         for (Skeleton::Bone& bone : skeleton->bones)
         {
            const Skeleton::Bone& base = stance.bones[boneId];
            bone.position = base.position;
            bone.rotation = base.rotation;
            bone.scale = base.scale;

            if (dst.positions.count(bone.name) + src.positions.count(bone.name) > 0)
            {
               glm::vec3 dstVal = dst.positions.count(bone.name) == 0 ? base.position : dst.positions.at(bone.name);
               glm::vec3 srcVal = src.positions.count(bone.name) == 0 ? base.position : src.positions.at(bone.name);
               bone.position = progress * dstVal + (1 - progress) * srcVal;
            }

            if (dst.rotations.count(bone.name) + src.rotations.count(bone.name) > 0)
            {
               glm::vec3 dstVal = dst.rotations.count(bone.name) == 0 ? base.rotation : dst.rotations.at(bone.name);
               glm::vec3 srcVal = src.rotations.count(bone.name) == 0 ? base.rotation : src.rotations.at(bone.name);
               bone.rotation = progress * dstVal + (1 - progress) * srcVal;
            }

            if (dst.scales.count(bone.name) + src.scales.count(bone.name) > 0)
            {
               glm::vec3 dstVal = dst.scales.count(bone.name) == 0 ? base.scale : dst.scales.at(bone.name);
               glm::vec3 srcVal = src.scales.count(bone.name) == 0 ? base.scale : src.scales.at(bone.name);
               bone.scale = progress * dstVal + (1 - progress) * srcVal;
            }

            ++boneId;
         }
      }

      // Run in a separate loop in the very off chance that a bone has a parent that was created
      // after it. Things will be a _little_ less stupid but still stupid if that occurs.
      boneId = 0;
      for (const auto& skeleton : controller.skeletons)
      {
         for (Skeleton::Bone& bone : skeleton->bones)
         {
            glm::mat4 matrix(1);
            if (boneId != 0)
            {
               // Assume this will always succeed
               const std::string& parent = stance.bones[boneId].parent;
               for (const auto& s : controller.skeletons)
               {
                  if (s->boneLookup.count(parent) > 0)
                  {
                     matrix = s->bones[s->boneLookup[parent]].matrix;
                  }
               }
            }

            matrix = glm::translate(matrix, bone.position);
            matrix = glm::rotate(matrix, RADIANS(bone.rotation.y), glm::vec3(0, 1, 0));
            matrix = glm::rotate(matrix, RADIANS(bone.rotation.x), glm::vec3(1, 0, 0));
            matrix = glm::rotate(matrix, RADIANS(bone.rotation.z), glm::vec3(0, 0, 1));
            matrix = glm::scale(matrix, bone.scale);
            bone.matrix = matrix;

            ++boneId;
         }
      }
   });

   // Update particle systems
   entities.Each<Engine::Transform, SimpleAnimationController>([&](Engine::Transform& transform, SimpleAnimationController& controller) {
      const State& state = controller.states[controller.current];

      if (controller.emitters)
      {
         for (const SimpleAnimationController::EmitterRef& ref : state.emitters)
         {
            if (controller.time >= ref.start && controller.time <= ref.end)
            {
               MultipleParticleEmitters::Emitter& system = controller.emitters->systems[ref.emitter];
               if (!system.active)
               {
                  system.Reset();
                  system.active = true;
               }
               system.update = true;
               system.render = true;

               for (const auto& s : controller.skeletons)
               {
                  if (s->boneLookup.count(ref.bone) != 0)
                  {
                     system.transform =
                        transform.GetMatrix() *
                        s->bones[s->boneLookup.at(ref.bone)].matrix *
                        glm::rotate(glm::mat4(1), RADIANS(180), glm::vec3{0, 1, 0});
                     break;
                  }
               }
            }
            else
            {
               controller.emitters->systems[ref.emitter].active = false;
            }
         }
      }
   });

   entities.Each<Skeleton, VoxModel>([&](Engine::Entity, Skeleton& skeleton, VoxModel& model) {
      size_t nBones = skeleton.bones.size();
      if (skeleton.bones.size() != model.mParts.size())
      {
         LOG_WARNING("Attached model and skeleton have a different amount of parts. Something may look strange");
         nBones = std::min(skeleton.bones.size(), model.mParts.size());
      }

      for (size_t b = 0; b < nBones; ++b)
      {
         model.mParts[b].transform = skeleton.bones[b].matrix;
      }
   });
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

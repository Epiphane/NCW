// By Thomas Steinke

#include <glm/ext.hpp>
#include <RGBBinding/BindingPropertyHelper.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
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
   : current(0)
   , time(0)
   , cooldown(0)
   , next(0)
   , transitionCurrent(0)
   , transitionStart(0)
   , transitionEnd(0)
   , emitterContainer(emitters)
{
}

void SimpleAnimationController::Reset()
{
   if (emitterContainer)
   {
      emitterContainer->systems.clear();
   }
   skeletons.clear();
   animations.clear();
   stances.clear();
   states.clear();
   current = 0;
}

void SimpleAnimationController::UpdateSkeletonStates()
{
   for (const Engine::ComponentHandle<SkeletonAnimations>& anims : animations)
   {
      anims->states.clear();
   }

   for (const auto& state : states)
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

         // If this is the first entity (owner), then set modified=true.
         found = true;

         SkeletonAnimations::State newState;
         newState.name = state.name;
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

         anims->states.emplace(state.name, std::move(newState));

         skipped.clear();
      }

      // Update effects
      for (const Emitter& emitter : state.particles)
      {
         (void)emitter;
         auto it = std::find_if(
            animations.begin(),
            animations.end(),
            [&](const auto& component) { return component->entity == emitter.entity; });
         assert(it != animations.end());

         Engine::ComponentHandle<SkeletonAnimations> anims = *it;
         std::vector<SkeletonAnimations::ParticleEffect>& effectDefs = anims->states[state.name].particles;

         SkeletonAnimations::ParticleEffect effectDef;
         effectDef.name = emitter.name;
         effectDef.bone = emitter.bone;
         effectDef.start = emitter.start;
         effectDef.end = emitter.end;
         BindingProperty config = emitter.Serialize();

         // Figure out what's been modified.
         std::string originalPath = Asset::Particle(effectDef.name);
         Maybe<BindingProperty> original = YAMLSerializer::DeserializeFile(originalPath);
         if (!original)
         {
            original.Failure().WithContext("Failed loading base particle system {path}", originalPath).Log();
            effectDef.modifications = config;
         }
         else
         {
            effectDef.modifications = BindingPropertyHelper::Difference(*original, config);
         }

         effectDefs.push_back(std::move(effectDef));
      }

      // Update events
      for (const auto& evt : state.events)
      {
         auto it = std::find_if(animations.begin(), animations.end(), [&](const auto& component) { return component->entity == evt.entity; });
         assert(it != animations.end());

         SkeletonAnimations& anims = **it;
         std::vector<SkeletonAnimations::Event>& eventDefs = anims.states[state.name].events;

         SkeletonAnimations::Event eventDef = evt;
         eventDefs.push_back(eventDef);
      }

      // Update events
      for (const auto& trans : state.transitions)
      {
         auto it = std::find_if(animations.begin(), animations.end(), [&](const auto& component) { return component->entity == trans.entity; });
         assert(it != animations.end());

         SkeletonAnimations& anims = **it;
         anims.states[state.name].transitions.push_back(trans);
      }
   }
}

void SimpleAnimationController::Play(const std::string& state, double startTime)
{
   auto it = std::find_if(states.begin(), states.end(), [&](const auto& s) { return s.name == state; });
   if (it == states.end())
   {
      return;
   }

   current = size_t(it - states.begin());
   time = startTime;
}

void SimpleAnimationController::TransitionTo(const std::string& state, double transitionTime, double startTime)
{
   auto it = std::find_if(states.begin(), states.end(), [&](const auto& s) { return s.name == state; });
   if (it == states.end())
   {
      return;
   }

   // If a transition is in flight, skip to the end of it.
   if (current != next)
   {
      current = next;
      time = transitionCurrent;
   }

   next = size_t(it - states.begin());
   transitionStart = startTime;
   transitionCurrent = startTime;
   transitionEnd = startTime + transitionTime;
}

void SimpleAnimationController::AddSkeleton(Engine::ComponentHandle<Skeleton> skeleton)
{
   skeletons.push_back(skeleton);

   const auto addSkeletonName = [&](const Skeleton::Bone& b) { return b; };
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
      auto stateIt = std::find_if(states.begin(), states.end(), [&](const auto& s) { return s.name == name; });
      if (stateIt == states.end())
      {
         states.push_back(State{});
         stateIt = states.end() - 1;
      }

      State& state = *stateIt;

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
               LOG_ERROR("Keyframe at time {time} has no match in the base state", k.time);
            }
            else
            {
               std::transform(k.positions.begin(), k.positions.end(), std::inserter(it->positions, it->positions.end()), addEntityName);
               std::transform(k.rotations.begin(), k.rotations.end(), std::inserter(it->rotations, it->rotations.end()), addEntityName);
               std::transform(k.scales.begin(), k.scales.end(), std::inserter(it->scales, it->scales.end()), addEntityName);
            }
         }
      }

      // Add particle effects
      for (const auto& particleDefinition : s.particles)
      {
         // Construct the base system.
         Emitter effect(
            Asset::Particle(particleDefinition.name),
            Asset::ParticleShaders(),
            Asset::Image("")
         );

         // Set MultipleParticleEmitter properties.
         effect.useEntityTransform = false;
         effect.update = false;
         effect.render = false;
         effect.ApplyConfiguration(Asset::Image(""), particleDefinition.modifications);

         // Set state properties.
         effect.entity = anims->entity;
         effect.bone = particleDefinition.bone;
         effect.start = particleDefinition.start;
         effect.end = particleDefinition.end;

         state.particles.push_back(std::move(effect));
      }

      // Add events
      for (const SkeletonAnimations::Event& eventDef : s.events)
      {
         Event evt = eventDef;
         evt.entity = anims->entity;

         state.events.push_back(std::move(evt));
      }

      // Add transitions
      for (const SkeletonAnimations::Transition& def : s.transitions)
      {
         Transition trans = def;
         trans.entity = anims->entity;

         state.transitions.push_back(std::move(trans));
      }
   }
}

void SimpleAnimationSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   bool seamlessLoop = true;
   entities.Each<AnimationSystemController>([&](AnimationSystemController& controller) {
      if (controller.paused)
      {
         dt = controller.nextTick;
         controller.nextTick = 0;
      }
      dt *= controller.speed;
      seamlessLoop = controller.seamlessLoop;
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
      if (controller.cooldown >= 0)
      {
         controller.cooldown -= dt;

         if (controller.cooldown < 0)
         {
            controller.time = 0;
         }
      }
      else
      {
         controller.time += dt / state.length;
         if (controller.time > 1)
         {
            controller.time = 1;
            controller.cooldown = seamlessLoop ? 0.0f : 1.0f;
         }
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
      float progress = 0.0f;
      if (dst.time > src.time)
      {
         progress = float(controller.time - src.time) / float(dst.time - src.time);
      }
      else if (isLastFrame && glm::epsilonNotEqual(src.time, 1.0, 0.1))
      {
         progress = float(controller.time - src.time) / float(1.0 - src.time);
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
      auto& emitters = controller.states[controller.current].particles;

      if (controller.emitterContainer)
      {
         controller.emitterContainer->systems.clear();
         for (SimpleAnimationController::Emitter& emitter : emitters)
         {
            controller.emitterContainer->systems.push_back(&emitter);
            if (controller.time >= emitter.start && controller.time <= emitter.end)
            {
               if (!emitter.active)
               {
                  emitter.Reset();
                  emitter.active = true;
               }
               emitter.update = true;
               emitter.render = true;

               for (const auto& s : controller.skeletons)
               {
                  if (s->boneLookup.count(emitter.bone) != 0)
                  {
                     emitter.transform =
                        transform.GetMatrix() *
                        s->bones[s->boneLookup.at(emitter.bone)].matrix *
                        glm::rotate(glm::mat4(1), RADIANS(180), glm::vec3{0, 1, 0});
                     break;
                  }
               }
            }
            else
            {
               emitter.active = false;
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

// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "../Event/NamedEvent.h"
#include "AnimationSystem.h"
#include "WalkAnimationSystem.h"
#include "BulletPhysicsSystem.h"

namespace CubeWorld
{

void WalkAnimationSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{}

void WalkAnimationSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   // Look for a max speed override in the animation and apply it.

   entities.Each<WalkSpeed, AnimationController>([&](WalkSpeed& walkParams, AnimationController& anim) {
      const AnimationController::State& state = anim.states[anim.current];
      const auto& keyframes = state.keyframes;
      size_t keyframeIndex = keyframes.size() - 1;
      while (anim.time < keyframes[keyframeIndex].time && keyframeIndex > 0)
      {
         keyframeIndex--;
      }

      walkParams.tempMaxSpeed = keyframes[keyframeIndex].maxSpeed;
   });

   // Okay, this is gonna get hella weird. Let me lay out the idea.
   //
   // You can do lots of things. You can also be _walking_ while you
   // do those things. How do we blend those two concepts?
   //
   // tl;dr what you're thinking about is what Unity calls an Avatar
   // mask. We're gonna try and emulate the same idea here. Basically,
   // if something is going on upstairs, we use the walk animation
   // but only apply transformations to everything below the hips.
   // Otherwise, the whole animation applies.

   const bool debug = false;
   entities.Each<WalkSpeed, BulletPhysics::ControlledBody, AnimationController>([&](WalkSpeed& walkParams, BulletPhysics::ControlledBody& body, AnimationController& anim) {
      const auto& linearVelocity = body.controller->getLinearVelocity();
      float speed = std::sqrt(linearVelocity.getX() * linearVelocity.getX() + linearVelocity.getZ() * linearVelocity.getZ());

      if (speed < 0.002)
      {
         anim.walkAnimationProgress = 0;
         if (debug)
         {
            DebugHelper::Instance().SetMetric("speed", 0);
            DebugHelper::Instance().SetMetric("walkAnimationProgress", anim.walkAnimationProgress);
            DebugHelper::Instance().SetMetric("baseBlend", 0);
            DebugHelper::Instance().SetMetric("walkBlend", 0);
            DebugHelper::Instance().SetMetric("runBlend", 0);
         }
         return;
      }

      // Here's where we get fuckin' weeeeeeeird:
      // walkAnimationProgress [0, 1) represents the progress we've made in the walk loop.
      // Each animation has a different length, so instead of keeping the time here we get
      // the length a blended animation _would_ be, and then progress along that track.
      if (anim.stateLookup.count("walk") == 0 || anim.stateLookup.count("run") == 0)
      {
         return;
      }

      // TODO parameterize
      const float walkSpeed = walkParams.walkSpeed;
      const float runSpeed = walkParams.runSpeed;


      const AnimationController::State& walk = anim.states[anim.stateLookup.at("walk")];
      const AnimationController::State& run = anim.states[anim.stateLookup.at("run")];

      float baseBlend = 0;
      float walkBlend = 0;
      float runBlend = 0;
      if (speed <= walkSpeed)
      {
         walkBlend = speed / walkSpeed;
         baseBlend = 1 - walkBlend;
      }
      else
      {
         runBlend = (speed - walkSpeed) / (runSpeed - walkSpeed);
         walkBlend = 1 - runBlend;
      }

      // Now progress the animation
      double cycleLength = 1.6 * baseBlend + walk.length * walkBlend + run.length * runBlend;
      anim.walkAnimationProgress += dt / cycleLength;
      while (anim.walkAnimationProgress > 1)
      {
         anim.walkAnimationProgress -= 1;
      }

      // Take the existing transformations and dull them according to the base blend,
      // so we can add walk/run on top.
      const AnimationController::State& state = anim.states[anim.current];
      const AnimationController::Stance& stance = anim.stances[state.stance];
      const auto& mask = state.movementMask;

      std::vector<bool> include;
      include.resize(anim.bones.size(), false);

      size_t boneId = 0;
      for (Engine::ComponentHandle<Skeleton>& skeleton : anim.skeletons)
      {
         for (Skeleton::Bone& bone : skeleton->bones)
         {
            if (mask.count(bone.name) != 0)
            {
               include[boneId] = mask.at(bone.name);
            }
            else
            {
               include[boneId] = include[stance.parents[boneId]];
            }

            if (include[boneId])
            {
               bone.position *= baseBlend;
               bone.rotation *= baseBlend;
               bone.scale *= baseBlend;
            }
            ++boneId;
         }
      }

      if (debug)
      {
         DebugHelper::Instance().SetMetric("cycleLength", cycleLength);
         DebugHelper::Instance().SetMetric("speed", speed);
         DebugHelper::Instance().SetMetric("walkAnimationProgress", anim.walkAnimationProgress);
         DebugHelper::Instance().SetMetric("baseBlend", baseBlend);
         DebugHelper::Instance().SetMetric("walkBlend", walkBlend);
         DebugHelper::Instance().SetMetric("runBlend", runBlend);
      }

      BlendState(include, anim, walk, walkBlend);
      BlendState(include, anim, run, runBlend);
   });
}

void WalkAnimationSystem::BlendState(
   const std::vector<bool>& mask,
   AnimationController& anim,
   const AnimationController::State& state,
   float blend
)
{
   if (blend == 0)
   {
      return;
   }

   double time = anim.walkAnimationProgress * state.length;

   size_t keyframeIndex = state.keyframes.size() - 1;
   while (time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
   {
      keyframeIndex--;
   }

   bool isLastFrame = (keyframeIndex == state.keyframes.size() - 1);

   const AnimationController::Keyframe& src = state.keyframes[keyframeIndex];
   const AnimationController::Keyframe& dst = isLastFrame ? state.keyframes[0] : state.keyframes[keyframeIndex + 1];
   const double dstTime = isLastFrame ? state.length : dst.time;
   float progress = 0.0f;
   if (dst.time > src.time)
   {
      progress = float(time - src.time) / float(dstTime - src.time);
   }
   else if (isLastFrame && glm::epsilonNotEqual(src.time, state.length, 0.001))
   {
      progress = float(time - src.time) / float(state.length - src.time);
   }

   size_t boneId = 0;
   for (Engine::ComponentHandle<Skeleton>& skeleton : anim.skeletons)
   {
      for (Skeleton::Bone& bone : skeleton->bones)
      {
         if (mask[boneId])
         {
            glm::vec3 position = progress * dst.positions[boneId] + (1 - progress) * src.positions[boneId];
            glm::vec3 rotation = progress * dst.rotations[boneId] + (1 - progress) * src.rotations[boneId];
            glm::vec3 scale = progress * dst.scales[boneId] + (1 - progress) * src.scales[boneId];
            bone.position += blend * position;
            bone.rotation += blend * rotation;
            bone.scale += blend * scale;
         }
         ++boneId;
      }
   }
}

}; // namespace CubeWorld

// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBLogger/Logger.h>
#include <Engine/Core/Config.h>

#include "../Components/VoxModel.h"
#include "BulletPhysicsSystem.h"
#include "AnimationSystem.h"

namespace CubeWorld
{

void AnimationSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{}

void AnimationSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   using Keyframe = AnimationController::Keyframe;
   using State = AnimationController::State;
   using Transition = SkeletonAnimations::Transition;

   // First, update skeletons.
   entities.Each<AnimationController>([&](Engine::Entity entity, AnimationController& controller) {
      // Check for an un-loaded skeleton
      if (controller.skeletons.empty())
      {
         return;
      }

      Engine::Transform& transform = *entity.Get<Engine::Transform>();
      glm::vec3 translateRoot{0, 0, 0};

      size_t prevState = controller.current;
      // Advance basic animation
      {
         State* state = &controller.states[controller.current];
         controller.time += dt;
         while (controller.time > state->length)
         {
            if (!state->loop)
            {
               controller.time = state->length;
               break;
            }

            // Looping!
            {
               // Move based on root's final state
               glm::vec3 finalPosition = state->keyframes[state->keyframes.size() - 1].positions[0];
               translateRoot += finalPosition - controller.lastBasePosition;
               controller.lastBasePosition = {0, 0, 0};
            }

            controller.time -= state->length;
            if (state->next != "")
            {
               const auto& it = controller.stateLookup.find(state->next);
               if (it == controller.stateLookup.end())
               {
                  LOG_ERROR("State {state} specified next={next}, which doesn't exist", state->name, state->next);
                  state->next = "";
               }
               else
               {
                  controller.current = controller.next = it->second;
                  state = &controller.states[controller.current];
               }
            }
         }

         const auto& keyframes = state->keyframes;
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
         else if (isLastFrame && glm::epsilonNotEqual(src.time, state->length, 0.1))
         {
            progress = float(controller.time - src.time) / float(state->length - src.time);
         }

         size_t boneId = 0;
         for (Engine::ComponentHandle<Skeleton>& skeleton : controller.skeletons)
         {
            for (Skeleton::Bone& bone : skeleton->bones)
            {
               bone.position = progress * dst.positions[boneId] + (1 - progress) * src.positions[boneId];
               bone.rotation = progress * dst.rotations[boneId] + (1 - progress) * src.rotations[boneId];
               bone.scale = progress * dst.scales[boneId] + (1 - progress) * src.scales[boneId];

               if (boneId == 0)
               {
                  translateRoot += bone.position - controller.lastBasePosition;
                  controller.lastBasePosition = bone.position;
               }

               boneId++;
            }
         }
      }

      {
         // Transitions!
         if (controller.current != controller.next)
         {
            State& state = controller.states[controller.next];
            controller.transitionCurrent += dt;
            float transitionProgress;
            if (controller.transitionCurrent < controller.transitionEnd)
            {
               transitionProgress = float(controller.transitionCurrent / (controller.transitionEnd - controller.transitionStart));
            }
            else
            {
               transitionProgress = 1;
               controller.current = controller.next;
               controller.time = controller.transitionCurrent;
            }

            double time = controller.transitionCurrent;
            while (time > state.length)
            {
               time -= state.length;
            }

            size_t keyframeIndex = state.keyframes.size() - 1;
            while (time < state.keyframes[keyframeIndex].time && keyframeIndex > 0)
            {
               keyframeIndex--;
            }

            bool isLastFrame = (keyframeIndex == state.keyframes.size() - 1);

            const Keyframe& src = state.keyframes[keyframeIndex];
            const Keyframe& dst = isLastFrame ? state.keyframes[0] : state.keyframes[keyframeIndex + 1];
            const double dstTime = isLastFrame ? state.length : dst.time;
            float progress = 0.0f;
            if (dst.time > src.time)
            {
               progress = float(time - src.time) / float(dstTime - src.time);
            }
            else if (isLastFrame && glm::epsilonNotEqual(src.time, state.length, 0.1))
            {
               progress = float(time - src.time) / float(state.length - src.time);
            }

            size_t boneId = 0;
            for (Engine::ComponentHandle<Skeleton>& skeleton : controller.skeletons)
            {
               for (Skeleton::Bone& bone : skeleton->bones)
               {
                  glm::vec3 position = progress * dst.positions[boneId] + (1 - progress) * src.positions[boneId];
                  glm::vec3 rotation = progress * dst.rotations[boneId] + (1 - progress) * src.rotations[boneId];
                  glm::vec3 scale = progress * dst.scales[boneId] + (1 - progress) * src.scales[boneId];
                  bone.position = transitionProgress * position + (1 - transitionProgress) * bone.position;
                  bone.rotation = transitionProgress * rotation + (1 - transitionProgress) * bone.rotation;
                  bone.scale = transitionProgress * scale + (1 - transitionProgress) * bone.scale;

                  if (boneId == 0)
                  {
                     translateRoot =
                        transitionProgress * (position - controller.lastTransitionPosition) +
                        (1 - transitionProgress) * translateRoot;
                     controller.lastTransitionPosition = position;
                     if (transitionProgress == 1)
                     {
                        controller.lastBasePosition = position;
                     }
                  }

                  boneId++;
               }
            }
         }

         // Compute new transitions
         if (controller.current == controller.next) {
            State& state = controller.states[controller.current];
            for (Transition& transition : state.transitions)
            {
               // Check triggers.
               bool valid = true;
               for (Transition::Trigger& trigger : transition.triggers)
               {
                  switch (trigger.type)
                  {
                  case Transition::Trigger::Type::GreaterThan:
                     valid &= controller.floatParams[trigger.parameter] >= trigger.doubleVal;
                     break;
                  case Transition::Trigger::Type::LessThan:
                     valid &= controller.floatParams[trigger.parameter] < trigger.doubleVal;
                     break;
                  case Transition::Trigger::Type::Bool:
                     valid &= controller.boolParams[trigger.parameter] == trigger.boolVal;
                     break;
                  default:
                     assert(false && "Unrecognized trigger type");
                  }
               }

               if (valid)
               {
                  controller.TransitionTo(transition.destination, transition.time);
                  break;
               }
            }
         }
      }

      // Move the actual transform based on movement in the root bone.
      translateRoot.y = 0;
      translateRoot = glm::normalize(transform.GetFlatDirection()) * glm::length(translateRoot);
      translateRoot *= transform.GetLocalScale();
      transform.SetLocalPosition(transform.GetLocalPosition() + translateRoot);

      auto maybeBody = entity.Get<BulletPhysics::ControlledBody>();
      if (maybeBody)
      {
         auto& trans = maybeBody->object->getWorldTransform();
         auto& origin = trans.getOrigin();
         origin.setValue(
            origin.getX() + translateRoot.x,
            origin.getY() + translateRoot.y,
            origin.getZ() + translateRoot.z
         );
         trans.setOrigin(origin);
      }

      // IMPORTANT: This is where the actual matrix transformation gets done, after all the
      // transitioning and looping work. Don't early out before here! If you do, nothing will
      // animate ever.
      size_t boneId = 0;
      std::vector<glm::mat4> matrixes;
      matrixes.resize(controller.bones.size(), glm::mat4(1));

      AnimationController::Stance& stance = controller.stances[controller.states[controller.current].stance];
      for (const Engine::ComponentHandle<Skeleton>& skeleton : controller.skeletons)
      {
         for (Skeleton::Bone& bone : skeleton->bones)
         {
            glm::mat4& matrix = matrixes[boneId];

            if (boneId != 0)
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
               assert(boneId > stance.parents[boneId]);
               matrix = matrixes[stance.parents[boneId]];
               matrix = glm::translate(matrix, bone.position);
            }

            matrix = glm::rotate(matrix, RADIANS(bone.rotation.y), glm::vec3(0, 1, 0));
            matrix = glm::rotate(matrix, RADIANS(bone.rotation.x), glm::vec3(1, 0, 0));
            matrix = glm::rotate(matrix, RADIANS(bone.rotation.z), glm::vec3(0, 0, 1));
            matrix = glm::scale(matrix, bone.scale);
            bone.matrix = matrix;

            ++boneId;
         }
      }

      UpdateEmitters(entities, controller, transform, controller.states[controller.current], false);
      if (prevState != controller.current)
      {
         UpdateEmitters(entities, controller, transform, controller.states[prevState], true);
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

void AnimationSystem::UpdateEmitters(
   Engine::EntityManager& entities,
   AnimationController& controller,
   const Engine::Transform& transform,
   AnimationController::State& state,
   bool updateAllTransforms
) const
{
   for (AnimationController::ParticleEffect& effect : state.effects)
   {
      bool updateTransform = updateAllTransforms;

      bool active = controller.time >= effect.start && controller.time <= effect.end;
      if (active)
      {
         if (!effect.spawned)
         {
            Engine::Entity entity = entities.Create();

            effect.spawned = entity.Add<Engine::Transform>(glm::vec3(0));
            auto emitter = entity.Add<ParticleEmitter>(effect.name);
            emitter->destroyOnComplete = true;
            emitter->active = true;
            emitter->update = true;
            emitter->render = true;
         }
         updateTransform = true;
      }

      if (updateTransform && effect.spawned)
      {
         for (const auto& s : controller.skeletons)
         {
            if (s->boneLookup.count(effect.bone) != 0)
            {
               effect.spawned->SetMatrix(transform.GetMatrix() * s->bones[s->boneLookup.at(effect.bone)].matrix);
               break;
            }
         }
      }

      if (!active)
      {
         // Release emitter
         effect.spawned = Engine::ComponentHandle<Engine::Transform>();
      }
   }
}

}; // namespace CubeWorld

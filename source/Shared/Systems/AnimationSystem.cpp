// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBLogger/Logger.h>
#include <Engine/Core/Config.h>

#include "AnimationSystem.h"

namespace CubeWorld
{

void BaseAnimationSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   using Keyframe = AnimationController::Keyframe;
   using State = AnimationController::State;
   using Transition = AnimationController::Transition;

   // First, update skeletons.
   entities.Each<AnimationController>([&](Engine::Entity /*entity*/, AnimationController& controller) {
      // Check for an un-loaded skeleton
      if (controller.skeletons.empty())
      {
         return;
      }

      // Advance basic animation
      if (mAnimate)
      {
         State* state = &controller.states[controller.current];
         controller.time += dt;
         while (controller.time >= state->length)
         {
            controller.time -= state->length;
            if (mTransitions && state->next != "")
            {
               const auto& it = controller.statesByName.find(state->next);
               if (it == controller.statesByName.end())
               {
                  LOG_ERROR("State %1 specified next='%2', which doesn't exist", state->name, state->next);
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
         const double dstTime = isLastFrame ? state->length : dst.time;
         const float progress = float(controller.time - src.time) / float(dstTime - src.time);

         size_t boneId = 0;
         for (Engine::ComponentHandle<DeprecatedSkeleton>& skeleton : controller.skeletons)
         {
            for (DeprecatedSkeleton::Bone& bone : skeleton->bones)
            {
               bone.position = progress * dst.positions[boneId] + (1 - progress) * src.positions[boneId];
               bone.rotation = progress * dst.rotations[boneId] + (1 - progress) * src.rotations[boneId];
               bone.scale = progress * dst.scales[boneId] + (1 - progress) * src.scales[boneId];
               boneId++;
            }
         }
      }

      if (mTransitions)
      {
         // Transitions!
         if (controller.current != controller.next)
         {
            State& state = controller.states[controller.next];
            controller.transitionCurrent = controller.transitionCurrent + dt;
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
            while (time >= state.length)
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
            const float progress = float(time - src.time) / float(dstTime - src.time);

            size_t boneId = 0;
            for (Engine::ComponentHandle<DeprecatedSkeleton>& skeleton : controller.skeletons)
            {
               for (DeprecatedSkeleton::Bone& bone : skeleton->bones)
               {
                  glm::vec3 position = progress * dst.positions[boneId] + (1 - progress) * src.positions[boneId];
                  glm::vec3 rotation = progress * dst.rotations[boneId] + (1 - progress) * src.rotations[boneId];
                  glm::vec3 scale = progress * dst.scales[boneId] + (1 - progress) * src.scales[boneId];
                  bone.position = transitionProgress * position + (1 - transitionProgress) * bone.position;
                  bone.rotation = transitionProgress * rotation + (1 - transitionProgress) * bone.rotation;
                  bone.scale = transitionProgress * scale + (1 - transitionProgress) * bone.scale;
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
                  case Transition::Trigger::FloatGte:
                     valid &= controller.floatParams[trigger.parameter] >= trigger.doubleVal;
                     break;
                  case Transition::Trigger::FloatLt:
                     valid &= controller.floatParams[trigger.parameter] < trigger.doubleVal;
                     break;
                  case Transition::Trigger::Bool:
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
   
      // IMPORTANT: This is where the actual matrix transformation gets done, after all the
      // transitioning and looping work. Don't early out before here! If you do, nothing will
      // animate ever.
      size_t boneId = 0;
      std::vector<glm::mat4> matrixes;
      matrixes.resize(controller.bones.size(), glm::mat4(1));

      AnimationController::Stance& stance = controller.stances[controller.states[controller.current].stance];
      for (size_t i = 0; i < controller.skeletons.size(); i ++)
      {
         Engine::ComponentHandle<DeprecatedSkeleton>& skeleton = controller.skeletons[i];
         for (DeprecatedSkeleton::Bone& bone : skeleton->bones)
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
            }

            matrix = glm::translate(matrix, bone.position);
            matrix = glm::rotate(matrix, RADIANS(bone.rotation.y), glm::vec3(0, 1, 0));
            matrix = glm::rotate(matrix, RADIANS(bone.rotation.x), glm::vec3(1, 0, 0));
            matrix = glm::rotate(matrix, RADIANS(bone.rotation.z), glm::vec3(0, 0, 1));
            matrix = glm::scale(matrix, bone.scale);
            bone.matrix = matrix;

            ++boneId;
         }

         if (skeleton->model)
         {
            size_t nBones = skeleton->bones.size();
            if (skeleton->bones.size() != skeleton->model->mParts.size())
            {
               LOG_WARNING("Attached model and skeleton have a different amount of parts. Something may look strange");
               nBones = std::min(skeleton->bones.size(), skeleton->model->mParts.size());
            }

            for (size_t b = 0; b < nBones; ++b)
            {
               skeleton->model->mParts[b].transform = skeleton->bones[b].matrix;
            }
         }
      }
   });
}

}; // namespace CubeWorld

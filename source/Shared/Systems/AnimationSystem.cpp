// By Thomas Steinke

#include <algorithm>
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
         for (Engine::ComponentHandle<AnimatedSkeleton>& skeleton : controller.skeletons)
         {
            for (AnimatedSkeleton::Bone& bone : skeleton->bones)
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
            for (Engine::ComponentHandle<AnimatedSkeleton>& skeleton : controller.skeletons)
            {
               for (AnimatedSkeleton::Bone& bone : skeleton->bones)
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
                     valid &= controller.floatParams[trigger.parameter] >= trigger.floatVal;
                     break;
                  case Transition::Trigger::FloatLt:
                     valid &= controller.floatParams[trigger.parameter] < trigger.floatVal;
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
      for (size_t i = 0; i < controller.skeletons.size(); i ++)
      {
         Engine::ComponentHandle<AnimatedSkeleton>& skeleton = controller.skeletons[i];
         for (size_t boneId = 0; boneId < skeleton->bones.size(); ++boneId)
         {
            skeleton->ComputeBoneMatrix(boneId);

            if (i > 0 && boneId == 0)
            {
               auto parentId = controller.skeletonParents[i];
               Engine::ComponentHandle<AnimatedSkeleton>& parent = controller.skeletons[parentId.first];
               skeleton->bones[boneId].matrix = parent->bones[parentId.second].matrix * skeleton->bones[boneId].matrix;
            }
         }

         if (skeleton->model)
         {
            size_t nBones = skeleton->bones.size();
            if (skeleton->bones.size() != skeleton->model->mParts.size())
            {
               LOG_WARNING("Attached model and skeleton have a different amount of parts. Something may look strange");
               nBones = std::min(skeleton->bones.size(), skeleton->model->mParts.size());
            }

            for (size_t boneId = 0; boneId < nBones; ++boneId)
            {
               skeleton->model->mParts[boneId].transform = skeleton->bones[boneId].matrix;
            }
         }
      }
   });
}

}; // namespace CubeWorld

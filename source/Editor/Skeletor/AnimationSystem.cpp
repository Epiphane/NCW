// By Thomas Steinke

#include <algorithm>
#include <RGBLogger/Logger.h>

#include "AnimationSystem.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

void AnimationSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   entities.Each<SkeletonCollection>([&](Engine::Entity, SkeletonCollection& collection) {
      for (size_t i = 0; i < collection.skeletons.size(); i++)
      {
         Engine::ComponentHandle<DeprecatedSkeleton>& skeleton = collection.skeletons[i];
         const DeprecatedSkeleton::Stance& stance = skeleton->stances[collection.stance];
         for (size_t boneId = 0; boneId < skeleton->bones.size(); ++boneId)
         {
            skeleton->bones[boneId].position = stance.bones[boneId].position;
            skeleton->bones[boneId].rotation = stance.bones[boneId].rotation;
            skeleton->bones[boneId].scale = stance.bones[boneId].scale;

            skeleton->ComputeBoneMatrix(boneId);

            if (i > 0 && boneId == 0)
            {
               size_t parent = 0;
               for (; parent < collection.skeletons.size(); ++parent)
               {
                  if (skeleton->parent == collection.skeletons[parent]->name)
                  {
                     break;
                  }
               }

               if (parent < collection.skeletons.size())
               {
                  auto& parentSkeleton = collection.skeletons[parent];
                  auto it = parentSkeleton->bonesByName.find(stance.parentBone);
                  if (it != parentSkeleton->bonesByName.end())
                  {
                     skeleton->bones[boneId].matrix = parentSkeleton->bones[it->second].matrix * skeleton->bones[boneId].matrix;
                  }
               }
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

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

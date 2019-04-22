// By Thomas Steinke

#include <algorithm>
#include <stack>
#include <RGBLogger/Logger.h>

#include "SkeletonSystem.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

void SkeletonCollection::AddSkeleton(const Engine::ComponentHandle<Skeleton>& skeleton)
{
   skeletons.push_back(skeleton);

   for (const auto& [name, def] : skeleton->stances)
   {
      std::string& parent = parents[name];
      if (!parent.empty())
      {
         // TODO: we could get into an unhandled situation:
         // Stance inheritance: A -> B -> C
         // Existing has A -> C, new skeleton defines A -> B and B -> C
         // Existing has A -> B -> C, new skeleton defines A -> C
         // Asserts because I don't want to spend the time thinking
         // through it right now.
         assert(parent == def.parent && "Stance parent mismatch");
      }
      else
      {
         parent = def.parent;
      }
   }
   dirty = true;
}

void SkeletonCollection::Reset()
{
   skeletons.clear();
   parents.clear();
   stance = "base";
   dirty = true;
}

void SkeletonCollection::SetStance(const std::string& s)
{
   stance = s;
   dirty = true;
}

void SkeletonSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   entities.Each<SkeletonCollection>([&](Engine::Entity, SkeletonCollection& collection) {
      if (collection.dirty)
      {
         // Construct a stack where the topmost value is of the lowest priority.
         std::stack<std::string> tree;
         for (std::string it = collection.stance; !it.empty(); it = collection.parents[it])
         {
            tree.push(it);
         }

         // Reset all the skeletons
         for (Engine::ComponentHandle<Skeleton> skeleton : collection.skeletons)
         {
            skeleton->bones.assign(skeleton->original.begin(), skeleton->original.end());
         }

         // Layer each stance back on top
         while (!tree.empty())
         {
            std::string it = tree.top();
            tree.pop();

            for (Engine::ComponentHandle<Skeleton> skeleton : collection.skeletons)
            {
               const Skeleton::Stance& stance = skeleton->stances[it];

               for (const auto&[bone, pos] : stance.positions)
               {
                  skeleton->bones[skeleton->boneLookup[bone]].position = pos;
               }
               for (const auto&[bone, rot] : stance.rotations)
               {
                  skeleton->bones[skeleton->boneLookup[bone]].rotation = rot;
               }
               for (const auto&[bone, scl] : stance.scales)
               {
                  skeleton->bones[skeleton->boneLookup[bone]].scale = scl;
               }
               for (const auto&[bone, parent] : stance.parents)
               {
                  skeleton->bones[skeleton->boneLookup[bone]].parent = parent;
               }
            }
         }

         for (Engine::ComponentHandle<Skeleton> skeleton : collection.skeletons)
         {
            for (Skeleton::Bone& bone : skeleton->bones)
            {
               if (bone.name == "root" && skeleton->parent == "")
               {
                  bone.matrix = glm::mat4(1);
               }
               else if (bone.name == "root")
               {
                  auto it = std::find_if(collection.skeletons.begin(), collection.skeletons.end(), [&](const auto& other) { return skeleton->parent == other->name; });
                  if (it == collection.skeletons.end())
                  {
                     LOG_WARNING("Skeleton '%1' claims to have parent '%2', which is not in the collection", skeleton->name, skeleton->parent);
                     bone.matrix = glm::mat4(1);
                  }
                  else
                  {
                     bone.matrix = (*it)->bones[(*it)->boneLookup[bone.parent]].matrix;
                  }
               }
               else
               {
                  bone.matrix = skeleton->bones[skeleton->boneLookup[bone.parent]].matrix;
               }

               Skeleton::Transform(bone.matrix, bone.position, bone.rotation, bone.scale);
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

         //collection.dirty = false;
      }
   });
}

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

// By Thomas Steinke

#include "EntityManager.h"

namespace CubeWorld
{

namespace Engine
{

Entity::ID EntityManager::MakeID(uint32_t index) const
{
   return Entity::ID(index, mEntityVersion[index]);
}

Entity EntityManager::Create()
{
   uint32_t index, version;
   if (mEntityFreeList.empty())
   {
      index = mNumEntities++;

      mEntityComponentMask.resize(mNumEntities);
      mEntityVersion.resize(mNumEntities);
      for (BasePool *pool : mComponentPools)
      {
         if (pool)
         {
            pool->expand(index + 1);
         }
      }
      version = mEntityVersion[index] = 1;
   }
   else
   {
      index = mEntityFreeList.back();
      mEntityFreeList.pop_back();
      version = mEntityVersion[index];
   }
   Entity entity(this, Entity::ID(index, version));
   // TODO emit an event.
   return entity;
}

Entity EntityManager::Clone(Entity original)
{
   assert(original.IsValid());
   Entity clone = Create();
   ComponentMask mask = GetComponentMask(original.id);
   for (size_t i = 0; i < mComponentHelpers.size(); ++i)
   {
      BaseComponentHelper *helper = mComponentHelpers[i];
      if (helper && mask.test(i))
      {
         helper->CloneComponent(original, clone);
      }
   }
   return clone;
}

}; // namespace Engine

}; // namespace CubeWorld

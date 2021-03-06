// By Thomas Steinke

#include "EntityManager.h"

namespace CubeWorld
{

namespace Engine
{

EntityManager::EntityManager(EventManager &events) : mEventManager(events)
{
}

EntityManager::~EntityManager()
{
   for (BasePool* pool : mComponentPools)
   {
      if (pool != nullptr)
      {
         delete pool;
      }
   }

   for (BaseComponentHelper* helper : mComponentHelpers)
   {
      if (helper != nullptr)
      {
         delete helper;
      }
   }

   mComponentPools.clear();
   mComponentHelpers.clear();
   mEntityComponentMask.clear();
   mEntityVersion.clear();
   mEntityFreeList.clear();
   mNumEntities = 0;
}

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
   mEventManager.Emit<EntityCreatedEvent>(entity);
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

void EntityManager::Destroy(Entity::ID id)
{
   assert_valid(id);
   Entity entity(this, id);

   uint32_t index = id.index();
   auto mask = mEntityComponentMask[index];
   for (size_t i = 0; i < mComponentHelpers.size(); ++i)
   {
      BaseComponentHelper* helper = mComponentHelpers[i];
      if (helper != nullptr && mask.test(i))
      {
         helper->RemoveComponent(entity);
      }
   }
   mEventManager.Emit<EntityDestroyedEvent>(entity);

   mEntityComponentMask[index].reset();
   mEntityVersion[index]++;
   mEntityFreeList.push_back(index);
}

Entity EntityManager::GetEntity(Entity::ID id)
{
   assert_valid(id);
   return Entity(this, id);
}

}; // namespace Engine

}; // namespace CubeWorld

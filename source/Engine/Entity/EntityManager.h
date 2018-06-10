// By Thomas Steinke

#pragma once

#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "../Core/Pool.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentHandle.h"

namespace CubeWorld
{

namespace Engine
{

//
// Helper class for handling component manipulation using types.
//
class BaseComponentHelper {
public:
   virtual ~BaseComponentHelper() {}
   virtual void RemoveComponent(Entity e) = 0;
   virtual void CloneComponent(Entity source, Entity target) = 0;
};

template <typename C>
class ComponentHelper : public BaseComponentHelper {
public:
   void RemoveComponent(Entity entity) override {
      entity.Remove<C>();
   }
   void CloneComponent(Entity source, Entity target) override {
      //target.assign_from_copy<C>(*(source.component<C>().get()));
   }
};

/**
 * EntityManager is in charge of managing all the entities themselves, as well as managing and locating their components.
 */
class EntityManager {
public:
   // Entity and component access.

   class EntityIterator : public std::iterator<std::input_iterator_tag, Entity::ID> {
   public:
      EntityIterator& operator++() // prefix
      {
         next();
         return *this;
      }

      void next()
      {
         while (mIndex < mCapacity && (mManager->mEntityComponentMask[mIndex] & mMask) != mMask)
         {
            ++mIndex;
         }
      }

      bool operator==(const EntityIterator& rhs)
      {
         return mIndex == rhs.mIndex;
      }

      bool operator!=(const EntityIterator& rhs)
      {
         return mIndex != rhs.mIndex;
      }

      Entity operator*()
      {
         return Entity(mManager, mManager->MakeID(mIndex));
      }

      const Entity operator*() const
      {
         return Entity(mManager, mManager->MakeID(mIndex));
      }

   private:
      friend class EntityManager;

      EntityIterator(EntityManager* manager, ComponentMask mask, size_t index)
         : mManager(manager)
         , mMask(mask)
         , mIndex(index)
         , mCapacity(manager->capacity())
      {};
      ~EntityIterator() {}

      EntityIterator& operator=(const EntityIterator& other)
      {
         mManager = other.mManager;
         mMask = other.mMask;
         mIndex = other.mIndex;
      }

      EntityManager* mManager;
      ComponentMask mMask;
      size_t mIndex;
      size_t mCapacity;
   };

   //
   // EntityView provides a collection of entities containing a specific ComponentMask.
   // This allows for iterating over all matching entities.
   //
   template<typename ...Components>
   class EntityView {
   public:
      using Iterator = EntityIterator;

      Iterator begin() { return Iterator(mManager, mMask, 0); }
      Iterator end() { return Iterator(mManager, mMask, manager->capacity()); }
      const Iterator begin() const { return Iterator(mManager, mMask, 0); }
      const Iterator end() const { return Iterator(mManager, mMask, mManager->capacity()); }

   private:
      friend class EntityManager;

      EntityView(EntityManager* manager, ComponentMask mask)
         : mManager(manager)
         , mMask(mask)
      {};

      EntityManager* mManager;
      ComponentMask mMask;
   };

   template<typename ...Components>
   EntityView<Components...> EntitiesWithComponents()
   {
      ComponentMask mask = MakeComponentMask<Components...>();
      return EntityIterator<Components...>(this, mask);
   }

   template<typename ...Components>
   void Each(std::function<void(Entity entity, Components&...)> fn)
   {
      return EntitiesWithComponents<Components...>().Each(f);
   }

public:
   // Entity lifecycle management.
   Entity Create()
   {
      uint32_t index, version;
      if (mEntityFreeList.empty())
      {
         index = mNumEntities++;
         version = mEntityVersion[index] = 1;

         mEntityComponentMask.reserve(mNumEntities);
         mEntityVersion.reserve(mNumEntities);
         for (BasePool *pool : mComponentPools)
         {
            if (pool)
            {
               pool->expand(index + 1);
            }
         }
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

   Entity Clone(Entity original)
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
   }

   Entity Get(Entity::ID id);

   Entity::ID MakeID(uint32_t index) const
   {
      return Entity::ID(index, mEntityVersion[index]);
   }

public:
   // Entity data access.
   inline void assert_valid(Entity::ID id) const
   {
      assert(id.index() < mEntityComponentMask.size() && "Entity::ID outside entity vector range");
      assert(mEntityVersion[id.index()] == id.version() && "Attempt to access Entity with a stale id");
   }

   ComponentMask GetComponentMask(Entity::ID id)
   {
      assert_valid(id);
      return mEntityComponentMask[id.index()];
   }

public:
   size_t size() { return mEntityComponentMask.size() - mEntityFreeList.size(); }

   size_t capacity() { return mEntityComponentMask.size(); }

private:
   // Entity data.
   uint32_t mNumEntities = 0;

   // Bitmask describing whether a particular component is enabled for an entity.
   std::vector<ComponentMask> mEntityComponentMask;
   // Version of each entity
   std::vector<uint32_t> mEntityVersion;
   // List of free entity slots
   std::vector<uint32_t> mEntityFreeList;

private:
   // Component data.

   // Pool of components. Each component is indexed by its family.
   std::vector<BasePool*> mComponentPools;
   // Each entry in this list is a ComponentHelper for the type indexed by its family.
   std::vector<BaseComponentHelper*> mComponentHelpers;

public:
   bool IsValid(Entity::ID id);
};

inline bool Entity::IsValid() const {
   return manager != nullptr && manager->IsValid(id);
}

}; // namespace Engine

}; // namespace CubeWorld

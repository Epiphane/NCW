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
#include "Transform.h"

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
   void CloneComponent(Entity /*source*/, Entity /*target*/) override {
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
         ++mIndex;
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
         return !(*this == rhs);
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

      EntityIterator(EntityManager* manager, ComponentMask mask, uint32_t index)
         : mManager(manager)
         , mMask(mask)
         , mIndex(index)
         , mCapacity(manager->capacity())
      {
         // Make sure the first element is valid.
         next();
      };
      ~EntityIterator() {}

      EntityIterator& operator=(const EntityIterator& other)
      {
         mManager = other.mManager;
         mMask = other.mMask;
         mIndex = other.mIndex;
         return *this;
      }

      EntityManager* mManager;
      ComponentMask mMask;
      uint32_t mIndex;
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
      Iterator end() { return Iterator(mManager, mMask, mManager->mNumEntities); }
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
      return EntityView<Components...>(this, mask);
   }

   template<typename ...Components>
   void Each(const std::function<void(Entity entity, Components&...)>& fn)
   {
      auto iter = EntitiesWithComponents<Components...>();
      for (auto entity : iter)
      {
         fn(entity, *(entity.template Get<Components>())...);
      }
   }

public:
   // Entity lifecycle management.
   Entity Create();

   Entity Clone(Entity original);

   Entity GetEntity(Entity::ID id);

   Entity::ID MakeID(uint32_t index) const;

   template<typename C, typename ...Args>
   ComponentHandle<C> Add(Entity::ID id, Args&& ...args)
   {
      assert_valid(id);
      const BaseComponent::Family family = C::GetFamily();
      assert(!mEntityComponentMask[id.index()].test(family));

      // Add to the component pool.
      if (mComponentPools.size() <= family)
      {
         mComponentPools.resize(family + 1, nullptr);
         mComponentHelpers.resize(family + 1, nullptr);
      }

      Pool<C> *pool;
      if (!mComponentPools[family])
      {
         pool = new Pool<C>();
         pool->expand(mNumEntities);
         mComponentPools[family] = pool;

         ComponentHelper<C> *helper = new ComponentHelper<C>();
         mComponentHelpers[family] = helper;
      }
      else
      {
         pool = static_cast<Pool<C>*>(mComponentPools[family]);
      }

      // Initialize the component inside the pool.
      ::new(pool->get(id.index())) C(std::forward<Args>(args) ...);

      // Set the component as active.
      mEntityComponentMask[id.index()].set(family);

      // Return handle to component.
      ComponentHandle<C> component(this, id);
      // TODO emit event.
      return component;
   }

   template<typename C>
   bool Has(Entity::ID id)
   {
      assert_valid(id);
      const BaseComponent::Family family = C::GetFamily();
      if (family >= mComponentPools.size())
      {
         return false;
      }
      return mComponentPools[family] && mEntityComponentMask[id.index()].test(family);
   }

   template<typename C>
   ComponentHandle<C> Get(Entity::ID id)
   {
      assert_valid(id);
      return Has<C>(id) ? ComponentHandle<C>(this, id) : ComponentHandle<C>();
   }

   template<typename C>
   void Remove(Entity::ID id)
   {
      assert_valid(id);
      const BaseComponent::Family family = C::GetFamily();

      BasePool* pool = mComponentPools[family];
      ComponentHandle<C> component(this, id);
      // TODO emit event.

      mEntityComponentMask[id.index()].reset(family);

      pool->destroy(id.index());
   }

public:
   // Entity data access.
   bool IsValid(Entity::ID id)
   {
      return id.index() < mNumEntities && mEntityVersion[id.index()] == id.version();
   }

   inline void assert_valid(Entity::ID id) const
   {
      assert(id.index() < mNumEntities && "Entity::ID outside entity vector range");
      assert(mEntityVersion[id.index()] == id.version() && "Attempt to access Entity with a stale id");
   }

   template<typename C>
   C* GetComponentPtr(Entity::ID id)
   {
      assert_valid(id);
      BasePool* pool = mComponentPools[C::GetFamily()];
      assert(pool);
      return static_cast<C*>(pool->get(id.index()));
   }

   ComponentMask GetComponentMask(Entity::ID id)
   {
      assert_valid(id);
      return mEntityComponentMask[id.index()];
   }

public:
   size_t size() { return mNumEntities - mEntityFreeList.size(); }

   size_t capacity() { return mNumEntities; }

private:
   // Entity data.
   // mNumEntities is the source of truth for number of entities registered.
   uint32_t mNumEntities = 0;

   // Bitmask describing whether a particular component is enabled for an entity.
   std::vector<ComponentMask> mEntityComponentMask;
   // Version of each entity
   std::vector<uint32_t> mEntityVersion;
   // List of free entity slots
   std::vector<uint32_t> mEntityFreeList;

private:
   // Component data.
   // mComponentPools.size() is the source of truth for number of components registered.

   // Pool of components. Each component is indexed by its family.
   std::vector<BasePool*> mComponentPools;
   // Each entry in this list is a ComponentHelper for the type indexed by its family.
   std::vector<BaseComponentHelper*> mComponentHelpers;
};

inline bool Entity::IsValid() const {
   return manager != nullptr && manager->IsValid(id);
}
   
template<typename C, typename ...Args>
ComponentHandle<C, EntityManager> Entity::Add(Args&& ...args)
{
   assert(IsValid());
   return manager->Add<C>(id, std::forward<Args>(args)...);
}

template<typename C>
bool Entity::Has()
{
   assert(IsValid());
   return manager->Has<C>(id);
}

template<typename C>
ComponentHandle<C, EntityManager> Entity::Get()
{
   assert(IsValid());
   return manager->Get<C>(id);
}

// Remove a component from this entity.
template<typename C>
void Entity::Remove()
{
   assert(IsValid() && Has<C>());
   return manager->Remove<C>(id);
}

}; // namespace Engine

}; // namespace CubeWorld

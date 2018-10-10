// By Thomas Steinke

#pragma once

#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "../Core/Pool.h"
#include "../Event/EventManager.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "Transform.h"

namespace CubeWorld
{

namespace Engine
{

//
// EntityManager is a long file with a lot of different parts, which are all mostly
// documented individually, but at a high level, each state has one EntityManager,
// which is responsible for managing the list of entities that exist in that state
// as well as all their associated data.
//
// An "Entity" is simply an ID, which serves as an index for locating the component
// data attached to it. Helper functions of Entity just call the EntityManager
// analogs of those functions, providing the ID as an argument.
//
// Components are organized in per-component arrays, meaning that PhysicsComponent[0]
// and PhysicsComponent[1] contains the physics-specific data for Entity 0 and 1,
// respectively, and live directly next to each other in data. These components are
// accessed through the EntityManager by using templated functions, for example:
// 
//    mEntities.Get<Transform>(myEntity);
//      or
//    myEntity.Get<Transform>();
//
// To iterate over all entities with components X and Y, simply call Each, as follows:
//
//    mEntities.Each<Transform, PhysicsBody>([&](Entity e, Transform& transform, PhysicsBody& body) {
//       // Contents of lambda, presumably making use of those components...
//    });
//
// Lastly, to create an entity and add components is simple. A complicated example is shown, to
// demonstrate creation of an entity, some components, and optionally using those components further.
//
//   Entity player = mEntities.Create();
//   player.Add<Transform>(glm::vec3(0, 10, 0));                    // Start the player at [0, 10, 0]
//   player.Add<SimplePhysics::Body>();                             // Subject the player to gravity
//   player.Add<SimplePhysics::Collider>(glm::vec3(0.8, 2.0, 0.8)); // Subject the player to collision
//   Engine::ComponentHandle<AnimatedSkeleton> skeleton = 
//      player.Add<AnimatedSkeleton>("player.json");                // Add an animated skeleton
//   skeleton->AddModel("torso", "body4.cub");                      // Add two models to the new skeleton
//   skeleton->AddModel("head", "elf-head-m02.cub");
//
//   Entity playerCamera = mEntities.Create(0, 0, 0);               // Create a camera to attach to the player.
//   playerCamera.Get<Transform>()->SetParent(player);
//
// EntityManager itself emits 4 different kind of events, at self-explanatory times:
//   - EntityCreatedEvent
//     Called when an entity is first created. It has no components attached to it at this time.
//
//   - ComponentAddedEvent<C>
//     Called when a C component is added to any entity. The event provides the entity, as well
//     as a handle to the component that has just been added.
//
//   - ComponentRemovedEvent<C>
//     Called immediately before a C component is removed from any entity. The event provides the
//     entity, as well as a handle to the component that is about to be removed.
//     NOTE: This is called for each component on an entity that is about to be destroyed.
//
//   - EntityDestroyedEvent
//     Called just before an entity is destroyed. It has no components attached to it at this time.

//
// Emitted when an entity is created.
//
struct EntityCreatedEvent : public Event<EntityCreatedEvent> {
   explicit EntityCreatedEvent(Entity entity) : entity(entity) {}
   virtual ~EntityCreatedEvent() {}

   Entity entity;
};

//
// Emitted when an entity is destroyed.
//
struct EntityDestroyedEvent : public Event<EntityDestroyedEvent> {
   explicit EntityDestroyedEvent(Entity entity) : entity(entity) {}
   virtual ~EntityDestroyedEvent() {}

   Entity entity;
};

//
// Emitted when any component is added to an entity.
//
template <typename C>
struct ComponentAddedEvent : public Event<ComponentAddedEvent<C>> {
   ComponentAddedEvent(Entity entity, ComponentHandle<C> component) :
      entity(entity), component(component) {}

   Entity entity;
   ComponentHandle<C> component;
};

//
// Emitted when any component is removed from an entity.
//
template <typename C>
struct ComponentRemovedEvent : public Event<ComponentRemovedEvent<C>> {
   ComponentRemovedEvent(Entity entity, ComponentHandle<C> component) :
      entity(entity), component(component) {}

   Entity entity;
   ComponentHandle<C> component;
};

namespace
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

}; // anonymous namespace

//
// EntityManager is in charge of managing all the entities themselves, as well as managing and locating their components.
//
class EntityManager {
public:
   explicit EntityManager(EventManager &events);
   virtual ~EntityManager();

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

   public:
      ~EntityIterator() {}

      EntityIterator& operator=(const EntityIterator& other)
      {
         mManager = other.mManager;
         mMask = other.mMask;
         mIndex = other.mIndex;
         return *this;
      }

      EntityIterator operator+(uint16_t inc)
      {
         EntityIterator iter = *this;
         while (inc--)
         {
            ++iter;
         }
         return iter;
      }

   private:
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

   //
   // Syntactic sugar for the below. This allows using a lambda, instead of predefining
   // a std::function when calling Each<A, B>([&](A a, B b) { ... });
   //
   template <typename T> struct identity { typedef T type; };

   template<typename ...Components>
   void Each(const typename identity<std::function<void(Entity entity, Components&...)>>::type fn)
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
   inline Entity Create(glm::vec3 position)
   {
      Entity result = Create();
      result.Add<Transform>(position);
      return result;
   }
   inline Entity Create(float x, float y, float z) { return Create(glm::vec3(x, y, z)); }
   Entity Clone(Entity original);

   void Destroy(Entity::ID id);

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
      mEventManager.Emit<ComponentAddedEvent<C>>(Entity(this, id), component);
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
      mEventManager.Emit<ComponentRemovedEvent<C>>(Entity(this, id), component);

      mEntityComponentMask[id.index()].reset(family);

      pool->destroy(id.index());
   }

public:
   // Entity data access.
   bool IsValid(Entity::ID id)
   {
      return id.index() < mNumEntities && mEntityVersion[id.index()] == id.version();
   }

#ifdef NDEBUG
   inline void assert_valid(Entity::ID) {}
#else
   inline void assert_valid(Entity::ID id) const
      uint32_t index = id.index();
      uint32_t version = id.version();
      assert(index < mNumEntities && "Entity::ID outside entity vector range");
      assert(mEntityVersion[id.index()] == version && "Attempt to access Entity with a stale id");
   }
#endif

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
   EventManager& mEventManager;

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
bool Entity::Has() const
{
   assert(IsValid());
   return manager->Has<C>(id);
}

template<typename C>
ComponentHandle<C, EntityManager> Entity::Get() const
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

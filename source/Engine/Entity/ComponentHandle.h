// By Thomas Steinke

#pragma once

#include "Component.h"
#include "Entity.h"

namespace CubeWorld
{

namespace Engine
{

class EntityManager;

/**
 * A ComponentHandle<C> wraps a component.
 *
 * It allows for safely accessing a component, invalidating it if:
 * - is removed from its entity.
 * - its entity is destroyed.
 */
template<typename C, typename EM = EntityManager>
class ComponentHandle {
public:
   typedef C ComponentType;

   ComponentHandle() : manager(nullptr) {};
   ComponentHandle(const ComponentHandle& other) : ComponentHandle(other.manager, other.id) {};

   bool IsValid() const;
   operator bool() const;

   // Pointer operators.
   C* operator->() const;

   // Dereference operators.
   C& operator*() const;

   // Getter
   C* get();
   const C* get() const;

   // Remove this component from its entity and clean it up.
   void remove();

   // Get the entity associated with this component.
   Entity GetEntity() const;

   bool operator==(const ComponentHandle<C>& other) const {
      return manager == other.manager && id == other.id;
   }

   bool operator!=(const ComponentHandle<C>& other) const {
      return !(this == other);
   }

private:
   friend class EntityManager;

   ComponentHandle(EM* manager, Entity::ID id)
      : id(id)
      , manager(manager)
   {};

   Entity::ID id;
   EM *manager;
};

//
// ComponentHandle implementations
//
template<typename C, typename EM>
inline bool ComponentHandle<C, EM>::IsValid() const
{
   return manager && manager->IsValid(id) && manager->template Has<C>(id);
}

template<typename C, typename EM>
inline ComponentHandle<C, EM>::operator bool() const
{ 
   return IsValid();
}

template<typename C, typename EM>
inline C* ComponentHandle<C, EM>::operator->() const
{
   assert(IsValid());
   return manager->template GetComponentPtr<C>(id);
}

template<typename C, typename EM>
inline C& ComponentHandle<C, EM>::operator*() const
{
   assert(IsValid());
   return *manager->template GetComponentPtr<C>(id);
}

template<typename C, typename EM>
inline C* ComponentHandle<C, EM>::get()
{
   if (!IsValid()) { return nullptr; }
   return manager->template GetComponentPtr<C>(id);
}

template<typename C, typename EM>
inline const C* ComponentHandle<C, EM>::get() const
{
   if (!IsValid()) { return nullptr; }
   return manager->template GetComponentPtr<C>(id);
}

template<typename C, typename EM>
inline void ComponentHandle<C, EM>::remove()
{
   assert(IsValid());
   return manager->template Remove<C>(id);
}

template<typename C, typename EM>
inline Entity ComponentHandle<C, EM>::GetEntity() const
{
   assert(IsValid());
   return manager->GetEntity(id);
}

}; // namespace Engine

}; // namespace CubeWorld

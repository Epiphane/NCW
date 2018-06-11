// By Thomas Steinke

#pragma once

#include <bitset>
#include <memory>

namespace CubeWorld
{

namespace Engine
{

class EntityManager;

template <typename C, typename EM>
class ComponentHandle;

/**
 * Base component class, only used for insertion into collections.
 *
 * Family is used for registration by the EntityManager.
 */
struct BaseComponent {
public:
   typedef size_t Family;

   // NOTE: Component memory is *always* managed by the EntityManager.
   // Use Entity::destroy() instead.
   void operator delete(void *) { fail(); }
   void operator delete[](void *) { fail(); }


protected:
   static void fail() {
#if defined(_HAS_EXCEPTIONS) || defined(__EXCEPTIONS)
      throw std::bad_alloc();
#else
      std::abort();
#endif
   }

   // This gets incremented with each unique call to Component<C>::GetFamily();
   static Family sNumFamilies;
};

/**
 * Component implementations should inherit from this.
 *
 * Components MUST provide a no-argument constructor.
 * Components SHOULD provide convenience constructors for initializing on assignment to an Entity::Id.
 *
 * This is a struct to imply that components should be data-only.
 *
 * Usage:
 *
 *     struct Position : public Component<Position> {
 *       Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
 *
 *       float x, y;
 *     };
 *
 * family() is used for registration.
 */
template <typename Derived>
struct Component : public BaseComponent {
public:
   typedef ComponentHandle<Derived, EntityManager> Handle;
   typedef ComponentHandle<const Derived, const EntityManager> ConstHandle;
   
   // Used internally for registration.
   // Defined here, because this is part of the template declaration,
   // so the compiler will consider it a different function for each component type.
   // That way each component class gets a different family.
   static Family GetFamily()
   {
      static Family family = sNumFamilies++;
      assert(family < MAX_COMPONENTS);
      return family;
   }
};

const uint32_t MAX_COMPONENTS = 64;
typedef std::bitset<MAX_COMPONENTS> ComponentMask;

template<typename C>
ComponentMask MakeComponentMask()
{
   ComponentMask mask;
   mask.set(Component<C>::GetFamily());
   return mask;
}

template<typename C1, typename C2, typename ...Components>
ComponentMask MakeComponentMask()
{
   return MakeComponentMask<C1>() | MakeComponentMask<C2, Components...>();
}

}; // namespace Engine

}; // namespace CubeWorld

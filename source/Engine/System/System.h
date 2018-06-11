// By Thomas Steinke

#pragma once

#include <string>

#include "../Core/Config.h"
#include "../Entity/EntityManager.h"

namespace CubeWorld
{

namespace Engine
{

class SystemManager;

class BaseSystem
{
public:
   typedef size_t Family;

   virtual ~BaseSystem() {}

   // Called once all the systems are added to the manager.
   // Typically used for setting up event handlers.
   virtual void Configure(EntityManager& /*entities*//*, EventManager& events*/) {}

   // Apply system behavior, called once per game step.
   virtual void Update(EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt) = 0;

   // This gets incremented with each unique call to System<C>::GetFamily();
   static Family sNumFamilies;
};

/**
 * Actual base class for implementing systems.
 *
 * struct MySystem : public System<MySystem> {
 *    void Update(EntityManager& entities, EventManager& events, TIMEDELTA dt)
 *    {
 *       ...
 *    }
 * }
 */
template<typename Derived>
class System : public BaseSystem {
public:
   virtual ~System() {}

private:
   friend class SystemManager;

   // Used internally for registration.
   // Defined here, because this is part of the template declaration,
   // so the compiler will consider it a different function for each component type.
   // That way each system class gets a different family.
   static Family GetFamily()
   {
      static Family family = sNumFamilies++;
      assert(family < MAX_COMPONENTS);
      return family;
   }
};

}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <RGBDesignPatterns/Pool.h>

namespace CubeWorld
{

namespace Engine
{

template <typename C, typename EM>
class ComponentHandle;

class EntityManager;

//
// Entity is a glorified wrapper for an ID, and other helper functions around that.
//
class Entity {
public:
   struct ID {
   public:
      ID() : _ind(0), _ver(0) {}
      //explicit ID(uint64_t id) : _id(id) {}
      ID(uint32_t index, uint32_t version) : _ind(index), _ver(version) {}

      uint64_t id() const { return uint64_t(_ind) | uint64_t(_ver) << 32UL; }

      bool operator == (const ID &other) const { return id() == other.id(); }
      bool operator != (const ID &other) const { return id() != other.id(); }
      bool operator < (const ID &other) const { return id() < other.id(); }

      uint32_t index() const { return _ind; }
      uint32_t version() const { return _ver; }
   private:
      uint32_t _ind;
      uint32_t _ver;
   };

public:
   Entity() = delete;
   Entity(EntityManager* manager, ID id) : manager(manager), id(id) {}
   Entity(const Entity& other) = default;
   Entity &operator=(const Entity &other) = default;

   // Add a component to this entity. Args represent any constructor args to the component.
   template<typename C, typename ...Args>
   ComponentHandle<C, EntityManager> Add(Args&& ...args);

   template<typename C>
   bool Has() const;

   template<typename C>
   ComponentHandle<C, EntityManager> Get() const;

   // Remove a component from this entity.
   template<typename C>
   void Remove();

private:
   friend class EntityManager;

   EntityManager* manager;

   // The ID of the entity. Consists of two components:
   // | version (32 bit) | index (32 bit) |
   //
   // `index` corresponds to an actual memory space that can be reused. If an entity is ever destroyed,
   // then its `index` can be reused, with a new `version`.
   ID id;

public:
   ID GetID() const { return id; }

public:
   // Falsey if the entity is invalid
   operator bool() const { return IsValid(); }

public:
   bool IsValid() const;
};

}; // namespace Engine

}; // namespace CubeWorld

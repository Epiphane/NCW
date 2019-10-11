// By Thomas Steinke

#pragma once

#include <Engine/Core/Timer.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"
#include "../Helpers/AABBTree.h"

namespace CubeWorld
{

namespace SimplePhysics
{

using bool3 = glm::vec<3, bool>;

struct Body : public Engine::Component<Body> {
   Body() : mass(1), velocity(0, 0, 0), lockPosition(false)
   {};
   
   double mass;
   glm::vec3 velocity;
   bool3 lockPosition;
};

// SimpleCollider represents a simple rectangular prism.
struct Collider : public Engine::Component<Collider> {
   Collider(glm::vec3 size, glm::vec3 center = glm::vec3(0), bool solid = true)
      : size(size)
      , center(center)
      , solid(solid)
   {};

   glm::vec3 size;
   glm::vec3 center;

   // If true, objects with a Body component cannot pass through.
   bool solid;
};

class System : public Engine::System<System>, public Engine::Receiver<System> {
public:
   /*
   class CollisionIterator : public std::iterator<std::input_iterator_tag, Engine::Entity::ID> {
   public:
      CollisionIterator& operator++();

      bool operator==(const CollisionIterator& rhs);
      bool operator!=(const CollisionIterator& rhs)
      {
         return !(*this == rhs);
      }

      Engine::Entity::ID operator*();
      const Engine::Entity::ID operator*() const;

   private:
      friend class System;
      CollisionIterator();

   public:
      ~CollisionIterator() {}

      CollisionIterator& operator=(const CollisionIterator& other);
      CollisionIterator operator+(uint16_t inc);
   };

   class CollisionView {
   public:
      CollisionIterator begin();
      CollisionIterator end();
      const CollisionIterator begin() const;
      const CollisionIterator end() const;

   private:
      friend class System;
      CollisionView(System& system);

      System& mSystem;
   };
   */

public:
   System() {}
   ~System() {}
   
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void Receive(const Engine::ComponentAddedEvent<Collider>& e);
   void Receive(const Engine::ComponentRemovedEvent<Collider>& e);
   
   // API for testing collision.
   //CollisionView Test(const glm::vec3& bottomLeft, const glm::vec3& rise);

private:
   friend class SimplePhysicsDebug;

   // A tree of bodies, for evaluating collisions quickly.
   AABBTree<Engine::Entity::ID> mBodies;

   // A list of entity IDs that are registered in mBodies.
   // Each time a body is added to the tree, the node references
   // an ID in this list.
   std::vector<Engine::Entity::ID> mEntities;
   std::vector<size_t> mEntitiesFreeList;

private:
   std::unique_ptr<DebugHelper::MetricLink> updateMetric, collisionMetric;
   Engine::Timer<100> mUpdateClock;
   Engine::Timer<100> mCollisionClock;
};

//
// SimplePhysicsDebug draws collision objects, for debugging physics.
//
class Debug : public Engine::System<Debug> {
public:
   Debug(bool active = true, Engine::Graphics::Camera* camera = nullptr) : mActive(active), mCamera(camera) {}
   ~Debug() {}

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void SetActive(bool active) { mActive = active; }
   bool IsActive() { return mActive; }

   void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }

private:
   bool mActive;

private:
   Engine::Graphics::Camera* mCamera;

   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace SimplePhysics

}; // namespace CubeWorld

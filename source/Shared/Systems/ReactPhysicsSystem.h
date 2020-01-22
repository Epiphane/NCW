// By Thomas Steinke

#pragma once

#include <reactphysics3d.h>

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

namespace ReactPhysics
{

struct Body : public Engine::Component<Body>
{
   Body(glm::vec3 size, float mass, rp3d::BodyType type)
      : size(size)
      , mass(mass)
      , type(type)
   {};

   glm::vec3 size;
   float mass;
   rp3d::BodyType type = rp3d::BodyType::STATIC;

   rp3d::RigidBody* body = nullptr;
   std::unique_ptr<rp3d::CollisionShape> collisionShape;
   rp3d::ProxyShape* shape = nullptr;
};

struct Collider : public Engine::Component<Collider>
{
   Collider(glm::vec3 size, float mass) : size(size), mass(mass) {};

   glm::vec3 size;
   float mass;

   rp3d::CollisionBody* collider = nullptr;
   rp3d::ProxyShape* shape = nullptr;
};

class System : public Engine::System<System>, public Engine::Receiver<System> {
public:
   System();
   ~System();
   
   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void Receive(const Engine::ComponentAddedEvent<Body>& e);
   void Receive(const Engine::ComponentRemovedEvent<Body>& e);
   void Receive(const Engine::ComponentAddedEvent<Collider>& e);
   void Receive(const Engine::ComponentRemovedEvent<Collider>& e);
   
   // API for testing collision.
   //CollisionView Test(const glm::vec3& bottomLeft, const glm::vec3& rise);

private:
   rp3d::DynamicsWorld world;

   // A tree of bodies, for evaluating collisions quickly.
   AABBTree<Engine::Entity::ID> mBodies;

   // A list of entity IDs that are registered in mBodies.
   // Each time a body is added to the tree, the node references
   // an ID in this list.
   std::vector<Engine::Entity::ID> mEntities;
   std::vector<size_t> mEntitiesFreeList;
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

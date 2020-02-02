// By Thomas Steinke

#include <deque>
#include <glad/glad.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <RGBDesignPatterns/Scope.h>
#include <RGBLogger/Logger.h>
#include <Engine/Graphics/Program.h>

#include "BulletPhysicsSystem.h"

namespace CubeWorld
{

namespace BulletPhysics
{

/*
btBoxShape* wepShape;
btPairCachingGhostObject* ghost;
*/

///
///
///
System::System()
{
   collisionConfiguration.reset(new btDefaultCollisionConfiguration());
   dispatcher.reset(new btCollisionDispatcher(collisionConfiguration.get()));
   broadphase.reset(new btDbvtBroadphase());
   solver.reset(new btSequentialImpulseConstraintSolver());
   world.reset(new btDiscreteDynamicsWorld(
      dispatcher.get(),
      broadphase.get(),
      solver.get(),
      collisionConfiguration.get())
   );
   world->setGravity(btVector3(0, -40, 0));

   broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

   // Try making something to collide with
   /*
   wepShape = new btBoxShape(btVector3{1.0f, 0.5f, 1.0f});
   ghost = new btPairCachingGhostObject();
   ghost->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));
   ghost->setCollisionShape(wepShape);
   ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
   world->addCollisionObject(ghost);
   //ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
   */
}

System::~System()
{}

///
///
///
void System::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
   events.Subscribe<Engine::ComponentAddedEvent<StaticBody>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<StaticBody>>(*this);
   events.Subscribe<Engine::ComponentAddedEvent<DynamicBody>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<DynamicBody>>(*this);
   events.Subscribe<Engine::ComponentAddedEvent<ControlledBody>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<ControlledBody>>(*this);

   updateMetric = DebugHelper::Instance().RegisterMetric("B3 Physics Update", [this]() -> std::string {
      return FormatString("%.2fms", mUpdateClock.Average() * 1000.0);
   });

   collisionMetric = DebugHelper::Instance().RegisterMetric("B3 Collision Checks", [this]() -> std::string {
      return FormatString("%.2fms", mCollisionClock.Average() * 1000.0);
   });
}

///
///
///
void System::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   mUpdateClock.Reset();
   world->stepSimulation(btScalar(dt));
   world->debugDrawWorld();
   mUpdateClock.Elapsed();

   mCollisionClock.Reset();
   entities.Each<Engine::Transform, ControlledBody>([&](Engine::Transform& transform, const ControlledBody& body) {
      btTransform trans = body.object->getWorldTransform();
      btVector3 position = trans.getOrigin();

      transform.SetLocalPosition(glm::vec3{position.getX(), position.getY(), position.getZ()});
   });
   mCollisionClock.Elapsed();

   /*
   btAlignedObjectArray<btCollisionObject*>& overlappingObjects = ghost->getOverlappingPairs();
   const int numObjects = overlappingObjects.size();
   for (int i = 0; i < numObjects; i++)
   {
      btCollisionObject* colObj = overlappingObjects[i];
      /// Do something with colObj
      (void*)colObj;
   }
   LOG_DEBUG("Overlapping: %1", numObjects);
   */
}

///
///
///
void System::AddBody(const glm::vec3& position, BodyBase& component)
{
   const auto& size = component.size;
   const auto& mass = component.mass;
   auto& motionState = component.motionState;
   auto& shape = component.shape;
   auto& body = component.body;

   shape.reset(new btBoxShape(btVector3{
      size.x / 2.0f,
      size.y / 2.0f,
      size.z / 2.0f,
   }));

   btVector3 localInertia(0, 0, 0);
   if (mass != 0)
   {
      shape->calculateLocalInertia(mass, localInertia);
   }

   btTransform transform;
   transform.setIdentity();
   transform.setOrigin(btVector3(position.x, position.y, position.z));
   motionState.reset(new btDefaultMotionState(transform));

   btRigidBody::btRigidBodyConstructionInfo cInfo(
      mass,
      motionState.get(),
      shape.get(),
      localInertia
   );

   body.reset(new btRigidBody(cInfo));
   body->setAngularFactor(btVector3(0, 0, 0));

   body->setUserIndex(-1);
   world->addRigidBody(body.get());
}

///
///
///
void System::RemoveBody(BodyBase& component)
{
   if (component.body)
   {
      world->removeRigidBody(component.body.get());
   }
}

///
///
///
void System::Receive(const Engine::ComponentAddedEvent<StaticBody>& e)
{
   AddBody(
      e.entity.Get<Engine::Transform>()->GetAbsolutePosition(),
      *e.component
   );
}

void System::Receive(const Engine::ComponentRemovedEvent<StaticBody>& e)
{
   RemoveBody(*e.component);
}

///
///
///
void System::Receive(const Engine::ComponentAddedEvent<DynamicBody>& e)
{
   AddBody(
      e.entity.Get<Engine::Transform>()->GetAbsolutePosition(),
      *e.component
   );
}

void System::Receive(const Engine::ComponentRemovedEvent<DynamicBody>& e)
{
   RemoveBody(*e.component);
}

///
///
///
void System::Receive(const Engine::ComponentAddedEvent<ControlledBody>& e)
{
   if (e.component->object)
   {
      world->addCollisionObject(e.component->object.get(), btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
   }

   if (e.component->controller)
   {
      world->addAction(e.component->controller.get());
   }
}

void System::Receive(const Engine::ComponentRemovedEvent<ControlledBody>& e)
{
   if (e.component->object)
   {
      world->removeCollisionObject(e.component->object.get());
   }

   if (e.component->controller)
   {
      world->removeAction(e.component->controller.get());
   }
}

}; // namespace BulletPhysics

}; // namespace CubeWorld

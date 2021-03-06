// By Thomas Steinke

#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
#endif
#include <btBulletDynamicsCommon.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/BroadphaseCollision/btBroadphaseProxy.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <Engine/Core/Timer.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"
#include "../Physics/VoxelHeightfieldTerrainShape.h"

namespace CubeWorld::BulletPhysics
{

//
// Bodies
//
struct BodyBase
{
   BodyBase(glm::vec3 size, float mass)
      : size(size)
      , mass(mass)
   {};

   glm::vec3 size;
   float mass;

   std::unique_ptr<btDefaultMotionState> motionState;
   std::unique_ptr<btCollisionShape> shape;
   std::unique_ptr<btRigidBody> body;
};

struct StaticBody : public Engine::Component<StaticBody>, public BodyBase
{
   StaticBody(glm::vec3 size)
      : BodyBase(size, 0.0f)
   {};
};

struct DynamicBody : public Engine::Component<DynamicBody>, public BodyBase
{
   DynamicBody(glm::vec3 size, float mass)
      : BodyBase(size, mass)
   {};
};

struct ControlledBody : public Engine::Component<ControlledBody>
{
   ControlledBody(
      std::unique_ptr<btCapsuleShape>&& shape,
      std::unique_ptr<btCollisionObject>&& object,
      std::unique_ptr<btKinematicCharacterController>&& controller
   )
   {
      this->shape = std::move(shape);
      this->object = std::move(object);
      this->controller = std::move(controller);
   };
   ControlledBody(const BindingProperty& data);

   std::unique_ptr<btCapsuleShape> shape;
   std::unique_ptr<btCollisionObject> object;
   std::unique_ptr<btKinematicCharacterController> controller;
};

struct VoxelHeightfieldBody : public Engine::Component<VoxelHeightfieldBody>
{
    VoxelHeightfieldBody(
        int16_t width,
        int16_t length,
        std::vector<short>&& heights
    )   : width(width)
        , length(length)
        , heights(std::move(heights))
    {};

    int16_t width;
    int16_t length;
    std::vector<short> heights;
    std::unique_ptr<btDefaultMotionState> motionState;
    std::unique_ptr<btCollisionShape> shape;
    std::unique_ptr<btRigidBody> body;
};

class Debug;

class System : public Engine::System<System>, public Engine::Receiver<System> {
public:
   System();
   ~System();

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

   void AddBody(const glm::vec3& position, BodyBase& component);
   void RemoveBody(BodyBase& component);

   void Receive(const Engine::ComponentAddedEvent<StaticBody>& e);
   void Receive(const Engine::ComponentRemovedEvent<StaticBody>& e);
   void Receive(const Engine::ComponentAddedEvent<DynamicBody>& e);
   void Receive(const Engine::ComponentRemovedEvent<DynamicBody>& e);
   void Receive(const Engine::ComponentAddedEvent<ControlledBody>& e);
   void Receive(const Engine::ComponentRemovedEvent<ControlledBody>& e);
   void Receive(const Engine::ComponentAddedEvent<VoxelHeightfieldBody>& e);
   void Receive(const Engine::ComponentRemovedEvent<VoxelHeightfieldBody>& e);

   void SetDebug(Debug* system) { mDebugSystem = system; }
   btCollisionWorld* GetWorld() const { return world.get(); }

private:
   std::unique_ptr<btCollisionConfiguration> collisionConfiguration;
   std::unique_ptr<btCollisionDispatcher> dispatcher;
   std::unique_ptr<btBroadphaseInterface> broadphase;
   std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
   std::unique_ptr<btDiscreteDynamicsWorld> world;

   // A list of entity IDs that are registered in mBodies.
   // Each time a body is added to the tree, the node references
   // an ID in this list.
   std::vector<Engine::Entity::ID> mEntities;
   std::vector<size_t> mEntitiesFreeList;

   // Metrics
   std::unique_ptr<DebugHelper::MetricLink> updateMetric, transformMetric;
   Engine::Timer<100> mUpdateClock;
   Engine::Timer<100> mTransformClock;

   // Debug
   Debug* mDebugSystem = nullptr;
};

}; // namespace CubeWorld::BulletPhysics

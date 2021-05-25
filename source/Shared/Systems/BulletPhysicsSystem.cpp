// By Thomas Steinke

#include <deque>
#include <glad/glad.h>

#include <RGBDesignPatterns/Scope.h>
#include <RGBLogger/Logger.h>
#include <Engine/Graphics/Program.h>

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include "../Physics/VoxelHeightfieldTerrainShape.h"
#include "AnimationSystem.h"
#include "BulletPhysicsSystem.h"

namespace CubeWorld
{

namespace BulletPhysics
{

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
    events.Subscribe<Engine::ComponentAddedEvent<VoxelHeightfieldBody>>(*this);
    events.Subscribe<Engine::ComponentRemovedEvent<VoxelHeightfieldBody>>(*this);

    updateMetric = DebugHelper::Instance().RegisterMetric("B3 Physics Update", [this]() -> std::string {
        return FormatString("%.2fms", mUpdateClock.Average() * 1000.0);
        });

    transformMetric = DebugHelper::Instance().RegisterMetric("B3 Transforms Updates", [this]() -> std::string {
        return FormatString("%.2fms", mTransformClock.Average() * 1000.0);
        });
}

///
///
///
void System::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
    mUpdateClock.Reset();
    world->stepSimulation(btScalar(dt));
    if (mDebug)
    {
        world->debugDrawWorld();
    }
    mUpdateClock.Elapsed();

    mTransformClock.Reset();
    entities.Each<Engine::Transform, ControlledBody>([&](Engine::Entity entity, Engine::Transform& transform, const ControlledBody& body) {
        btTransform trans = body.object->getWorldTransform();
        btVector3 position = trans.getOrigin();

        DebugHelper::Instance().SetMetric("Position", FormatString("{} {} {}", position.getX(), position.getY(), position.getZ()));

        glm::vec3 adjustedPosition = glm::vec3{ position.getX(), position.getY(), position.getZ() };
        adjustedPosition.y -= body.shape->getHalfHeight();

        if (entity.Has<AnimationController>())
        {
            const AnimationController& controller = *entity.Get<AnimationController>();
            adjustedPosition.y += controller.lastBasePosition.y * transform.GetLocalScale().y;
        }

        transform.SetLocalPosition(adjustedPosition);
        });

    entities.Each<Engine::Transform, DynamicBody>([&](Engine::Entity, Engine::Transform& transform, const DynamicBody& body) {
        btTransform trans = body.body->getWorldTransform();
        btVector3 position = trans.getOrigin();

        glm::vec3 adjustedPosition = glm::vec3{ position.getX(), position.getY(), position.getZ() };

        transform.SetLocalPosition(adjustedPosition);
        });
    mTransformClock.Elapsed();
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
    AddBody(e.entity.Get<Engine::Transform>()->GetAbsolutePosition(), *e.component);
    assert(sizeof(int) >= sizeof(uint32_t) && "int size is unexpected for this platform");
    e.component->body->setUserIndex((int)e.entity.GetID().index());
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
    AddBody(e.entity.Get<Engine::Transform>()->GetAbsolutePosition(), *e.component);
    assert(sizeof(int) >= sizeof(uint32_t) && "int size is unexpected for this platform");
    e.component->body->setUserIndex((int)e.entity.GetID().index());
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
        assert(sizeof(int) >= sizeof(uint32_t) && "int size is unexpected for this platform");
        e.component->object->setUserIndex((int)e.entity.GetID().index());
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

///
///
///
void System::Receive(const Engine::ComponentAddedEvent<VoxelHeightfieldBody>& e)
{
    const auto& width = e.component->width;
    const auto& length = e.component->length;
    const auto& heights = e.component->heights;
    auto& motionState = e.component->motionState;
    auto& shape = e.component->shape;
    auto& body = e.component->body;

    shape.reset(new VoxelHeightfieldTerrainShape(
        width,
        length,
        heights.data(),
        1,
        0,  // TODO figure out min
        48 // TODO figure out max
    ));
    /*
    shape.reset(new btHeightfieldTerrainShape(
        width,
        length,
        heights.data(),
        1.0,
        0,  // TODO figure out min
        48, // TODO figure out max
        1, PHY_SHORT, false
    ));
    */

    btVector3 localInertia(0, 0, 0);
    btTransform transform;
    transform.setIdentity();
    glm::vec3 position = e.entity.Get<Engine::Transform>()->GetAbsolutePosition();
    position.x += width / 2 - 0.5f;
    position.z += length / 2 - 0.5f;
    position.y += 23.5;
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    motionState.reset(new btDefaultMotionState(transform));

    body.reset(new btRigidBody(0 /* mass */, motionState.get(), shape.get(), localInertia));
    body->setUserIndex(-1);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    world->addRigidBody(body.get());
}

void System::Receive(const Engine::ComponentRemovedEvent<VoxelHeightfieldBody>& e)
{
    if (e.component->body)
    {
        world->removeRigidBody(e.component->body.get());
    }
}

}; // namespace BulletPhysics

}; // namespace CubeWorld

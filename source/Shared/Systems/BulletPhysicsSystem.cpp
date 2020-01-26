// By Thomas Steinke

#include <deque>
#include <glad/glad.h>

#include <RGBDesignPatterns/Scope.h>
#include <RGBLogger/Logger.h>
#include <Engine/Graphics/Program.h>

#include "BulletPhysicsSystem.h"

namespace CubeWorld
{

namespace BulletPhysics
{

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
   world->setGravity(btVector3(0, -10, 0));
}

System::~System()
{}

void System::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
   events.Subscribe<Engine::ComponentAddedEvent<Body>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<Body>>(*this);
   events.Subscribe<Engine::ComponentAddedEvent<Collider>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<Collider>>(*this);
}

void System::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   world->stepSimulation(btScalar(dt));

   entities.Each<Engine::Transform, Body>([&](Engine::Transform& transform, Body& body) {
      btTransform trans = body.body->getCenterOfMassTransform();
      btVector3 position = trans.getOrigin();

      transform.SetLocalPosition(glm::vec3{position.getX(), position.getY(), position.getZ()});
   });
}

void System::Receive(const Engine::ComponentAddedEvent<Body>& e)
{
   glm::vec3 pos = e.entity.Get<Engine::Transform>()->GetAbsolutePosition();
   const auto& size = e.component->size;
   const auto& mass = e.component->mass;
   auto& motionState = e.component->motionState;
   auto& shape = e.component->shape;
   auto& body = e.component->body;

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
   transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
   motionState.reset(new btDefaultMotionState(transform));

   btRigidBody::btRigidBodyConstructionInfo cInfo(
      mass,
      motionState.get(),
      shape.get(),
      localInertia
   );

   body.reset(new btRigidBody(cInfo));

   body->setUserIndex(-1);
   world->addRigidBody(body.get());
}

void System::Receive(const Engine::ComponentRemovedEvent<Body>& e)
{
   world->removeRigidBody(e.component->body.get());
}

void System::Receive(const Engine::ComponentAddedEvent<Collider>&)
{
   /*
   glm::vec3 pos = e.entity.Get<Engine::Transform>()->GetAbsolutePosition();

   rp3d::Vector3 initPosition(pos.x, pos.y, pos.z);
   rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
   rp3d::Transform transform(initPosition, initOrientation);

   rp3d::CollisionBody* collider = world.createCollisionBody(transform);
   e.component->collider = collider;

   rp3d::BoxShape shape(rp3d::Vector3(e.component->size.x, e.component->size.y, e.component->size.z));
   e.component->shape = collider->addCollisionShape(&shape, rp3d::Transform::identity(), e.component->mass);
   */
}

void System::Receive(const Engine::ComponentRemovedEvent<Collider>&)
{
   //world.destroyCollisionBody(e.component->collider);
}

///
///
///
std::unique_ptr<Engine::Graphics::Program> Debug::program = nullptr;

///
///
///

void Debug::Configure(Engine::EntityManager&, Engine::EventManager&)
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/PhysicsDebug.vert", "Shaders/PhysicsDebug.geom", "Shaders/PhysicsDebug.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading Physics Debug shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);
      program->Uniform("uProjMatrix");
      program->Uniform("uViewMatrix");
      program->Uniform("uModelMatrix");
      program->Uniform("uPosition");
      program->Uniform("uSize");
   }
}

///
///
///
void Debug::Update(Engine::EntityManager&, Engine::EventManager&, TIMEDELTA)
{
   if (!mActive)
   {
      return;
   }

   /*
   BIND_PROGRAM_IN_SCOPE(program);

   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   glm::mat4 model(1);
   program->UniformMatrix4f("uProjMatrix", perspective);
   program->UniformMatrix4f("uViewMatrix", view);
   program->UniformMatrix4f("uModelMatrix", model);

   entities.Each<Engine::Transform, Collider>([&](Engine::Entity, Engine::Transform& transform, Collider& collider) {
      glm::vec3 pos = transform.GetAbsolutePosition();
      program->UniformVector3f("uPosition", pos);
      program->UniformVector3f("uSize", collider.size);

      glDrawArrays(GL_POINTS, 0, 1);

      CHECK_GL_ERRORS();
   });
   */
}

}; // namespace BulletPhysics

}; // namespace CubeWorld

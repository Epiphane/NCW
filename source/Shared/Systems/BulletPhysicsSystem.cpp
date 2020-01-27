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
   world->setGravity(btVector3(0, -40, 0));
}

System::~System()
{}

void System::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
   events.Subscribe<Engine::ComponentAddedEvent<StaticBody>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<StaticBody>>(*this);
   events.Subscribe<Engine::ComponentAddedEvent<DynamicBody>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<DynamicBody>>(*this);
   events.Subscribe<Engine::ComponentAddedEvent<Collider>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<Collider>>(*this);

   updateMetric = DebugHelper::Instance().RegisterMetric("B3 Physics Update", [this]() -> std::string {
      return FormatString("%.2fms", mUpdateClock.Average() * 1000.0);
   });

   collisionMetric = DebugHelper::Instance().RegisterMetric("B3 Collision Checks", [this]() -> std::string {
      return FormatString("%.2fms", mCollisionClock.Average() * 1000.0);
   });
}

void System::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   mUpdateClock.Reset();
   world->stepSimulation(btScalar(dt));
   mUpdateClock.Elapsed();

   mCollisionClock.Reset();
   entities.Each<Engine::Transform, DynamicBody>([&](Engine::Transform& transform, const DynamicBody& body) {
      btTransform trans = body.body->getCenterOfMassTransform();
      btVector3 position = trans.getOrigin();

      transform.SetLocalPosition(glm::vec3{position.getX(), position.getY(), position.getZ()});
   });
   mCollisionClock.Elapsed();
}

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

void System::RemoveBody(BodyBase& component)
{
   if (component.body)
   {
      world->removeRigidBody(component.body.get());
   }
}

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

void System::Receive(const Engine::ComponentAddedEvent<Collider>&)
{
}

void System::Receive(const Engine::ComponentRemovedEvent<Collider>&)
{
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

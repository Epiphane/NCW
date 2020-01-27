// By Thomas Steinke

#include <deque>
#include <glad/glad.h>

#include <RGBDesignPatterns/Scope.h>
#include <RGBLogger/Logger.h>
#include <Engine/Graphics/Program.h>

#include "ReactPhysicsSystem.h"

namespace CubeWorld
{

namespace ReactPhysics
{

System::System()
   : world(rp3d::Vector3(0, 4 * -9.81, 0))
{
   world.setNbIterationsPositionSolver(2);
   world.setNbIterationsVelocitySolver(5);
}

System::~System()
{}

void System::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
   events.Subscribe<Engine::ComponentAddedEvent<DynamicBody>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<DynamicBody>>(*this);
   events.Subscribe<Engine::ComponentAddedEvent<Body>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<Body>>(*this);
   events.Subscribe<Engine::ComponentAddedEvent<Collider>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<Collider>>(*this);
}

void System::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   world.update((rp3d::decimal)dt);

   entities.Each<Engine::Transform, Body>([&](Engine::Transform& transform, Body& body) {
      auto position = body.body->getTransform().getPosition();

      transform.SetLocalPosition(glm::vec3{position.x, position.y, position.z});
   });
}

void System::Receive(const Engine::ComponentAddedEvent<DynamicBody>& e)
{
   glm::vec3 pos = e.entity.Get<Engine::Transform>()->GetAbsolutePosition();

   rp3d::Vector3 initPosition(pos.x, pos.y, pos.z);
   rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
   rp3d::Transform transform(initPosition, initOrientation);

   rp3d::RigidBody* body = world.createRigidBody(transform);
   body->setType(e.component->type);

   const auto& size = e.component->size;
   e.component->body = body;
   e.component->collisionShape.reset(new rp3d::BoxShape(rp3d::Vector3{size.x, size.y, size.z}));
   e.component->shape = body->addCollisionShape(e.component->collisionShape.get(), rp3d::Transform::identity(), e.component->mass);
}

void System::Receive(const Engine::ComponentRemovedEvent<DynamicBody>& e)
{
   world.destroyRigidBody(e.component->body);
}

void System::Receive(const Engine::ComponentAddedEvent<Body>& e)
{
   glm::vec3 pos = e.entity.Get<Engine::Transform>()->GetAbsolutePosition();

   rp3d::Vector3 initPosition(pos.x, pos.y, pos.z);
   rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
   rp3d::Transform transform(initPosition, initOrientation);

   rp3d::RigidBody* body = world.createRigidBody(transform);
   body->setType(e.component->type);

   const auto& size = e.component->size;
   e.component->body = body;
   e.component->collisionShape.reset(new rp3d::BoxShape(rp3d::Vector3{size.x, size.y, size.z}));
   e.component->shape = body->addCollisionShape(e.component->collisionShape.get(), rp3d::Transform::identity(), e.component->mass);
}

void System::Receive(const Engine::ComponentRemovedEvent<Body>& e)
{
   world.destroyRigidBody(e.component->body);
}

void System::Receive(const Engine::ComponentAddedEvent<Collider>& e)
{
   glm::vec3 pos = e.entity.Get<Engine::Transform>()->GetAbsolutePosition();

   rp3d::Vector3 initPosition(pos.x, pos.y, pos.z);
   rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
   rp3d::Transform transform(initPosition, initOrientation);

   rp3d::CollisionBody* collider = world.createCollisionBody(transform);
   e.component->collider = collider;

   rp3d::BoxShape shape(rp3d::Vector3(e.component->size.x, e.component->size.y, e.component->size.z));
   e.component->shape = collider->addCollisionShape(&shape, rp3d::Transform::identity(), e.component->mass);
}

void System::Receive(const Engine::ComponentRemovedEvent<Collider>& e)
{
   world.destroyCollisionBody(e.component->collider);
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

}; // namespace SimplePhysics

}; // namespace CubeWorld

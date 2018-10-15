// By Thomas Steinke

#include <deque>
#include <glad/glad.h>

#include <Engine/Core/Scope.h>
#include <Engine/Logger/Logger.h>
#include <Engine/Graphics/Program.h>

#include "../Event/NamedEvent.h"
#include "../Systems/SimplePhysicsSystem.h"

namespace CubeWorld
{

namespace SimplePhysics
{

namespace
{

using TransformHandle = Engine::ComponentHandle<Engine::Transform>;
using BodyHandle = Engine::ComponentHandle<Body>;
using ColliderHandle = Engine::ComponentHandle<Collider>;

bool TestAABB(glm::vec3 positionA, glm::vec3 plusSizeA, glm::vec3 minusSizeA, glm::vec3 positionB, glm::vec3 plusSizeB, glm::vec3 minusSizeB)
{
   glm::vec3 minA = positionA - minusSizeA;
   glm::vec3 minB = positionB - minusSizeB;
   glm::vec3 maxA = positionA + plusSizeA;
   glm::vec3 maxB = positionB + plusSizeB;
   if (
      minA.x >= maxB.x || minB.x >= maxA.x ||
      minA.y >= maxB.y || minB.y >= maxA.y ||
      minA.z >= maxB.z || minB.z >= maxA.z
   ) {
      return false;
   }

   return true;
}

bool Overlapping(const Engine::Entity& entityA, const Engine::Entity& entityB)
{
   TransformHandle transformA = entityA.Get<Engine::Transform>();
   TransformHandle transformB = entityB.Get<Engine::Transform>();
   ColliderHandle colliderA = entityA.Get<Collider>();
   ColliderHandle colliderB = entityB.Get<Collider>();
   if (!colliderA || !colliderB)
   {
      return false;
   }

   // First, radius test.
   // TODO size is extended out from both sides of the center, so this check is overly sensitive.
   /*double r1 = colliderA->size.length();
   double r2 = colliderB->size.length();
   double dist = (transformB->GetAbsolutePosition() - transformA->GetAbsolutePosition()).length();
   if (dist > r1 + r2)
   {
      return false;
   }*/

   // Then, AABB test.
   return TestAABB(
      transformA->GetAbsolutePosition(),
      colliderA->size / 2.0f,
      colliderA->size / 2.0f,
      transformB->GetAbsolutePosition(),
      colliderB->size / 2.0f,
      colliderB->size / 2.0f
   );
}

//
// Notes:
// - entityA is always assumed to be movable.
//
void ResolveCollision(const Engine::Entity& entityA, const Engine::Entity& entityB, TIMEDELTA dt)
{
   TransformHandle transformA = entityA.Get<Engine::Transform>();
   TransformHandle transformB = entityB.Get<Engine::Transform>();
   ColliderHandle colliderA = entityA.Get<Collider>();
   ColliderHandle colliderB = entityB.Get<Collider>();
   BodyHandle bodyA = entityA.Get<Body>();
   BodyHandle bodyB = entityB.Get<Body>();
   assert(transformA);
   assert(transformB);
   assert(colliderA && colliderA->solid);
   assert(colliderB && colliderB->solid);
   assert(bodyA);
   // TODO lol
   assert(!bodyB);

   // Before we actually back off, try just stepping up <= one unit on the y-axis.
   glm::vec3 halfSizeA = colliderA->size / 2.0f;
   glm::vec3 halfSizeB = colliderB->size / 2.0f;
   glm::vec3 positionA = transformA->GetLocalPosition();
   glm::vec3 positionB = transformB->GetLocalPosition();
   float stepSize = (positionB.y + halfSizeB.y) - (positionA.y - halfSizeA.y);
   if (stepSize <= 1.2f)
   {
      transformA->SetLocalPosition(positionA + glm::vec3(0, stepSize / 3.0f, 0));
      bodyA->velocity.y = 0;
      if (bodyA->velocity.x != 0 || bodyA->velocity.z != 0)
      {
         float speed = std::max(0.0f, bodyA->velocity.length() - 2.0f);
         bodyA->velocity = glm::normalize(bodyA->velocity) * speed;
      }
      return;
   }

   // Step back until we're not colliding.
   const uint8_t STEPS = 8;
   TIMEDELTA step = dt / STEPS;
   uint8_t remaining = STEPS;

   positionA -= float(dt) * bodyA->velocity;
   glm::vec3 velocityA = float(step) * bodyA->velocity;
   while (remaining > 0 && !TestAABB(positionA + velocityA, halfSizeA, halfSizeA, positionB, halfSizeB, halfSizeB))
   {
      positionA += velocityA;
      remaining--;
   }

   transformA->SetLocalPosition(positionA);

   // Figure out what's colliding and stop moving that direction.
   if (TestAABB(positionA + glm::vec3(dt * bodyA->velocity.x, 0, 0), halfSizeA, halfSizeA, positionB, halfSizeB, halfSizeB))
   {
      bodyA->velocity.x = 0;
   }
   if (TestAABB(positionA + glm::vec3(0, dt * bodyA->velocity.y, 0), halfSizeA, halfSizeA, positionB, halfSizeB, halfSizeB))
   {
      bodyA->velocity.y = 0;
   }
   if (TestAABB(positionA + glm::vec3(0, 0, dt * bodyA->velocity.z), halfSizeA, halfSizeA, positionB, halfSizeB, halfSizeB))
   {
      bodyA->velocity.z = 0;
   }

   transformA->SetLocalPosition(positionA + float(step) * remaining * bodyA->velocity);
}

}; // anonymous namespace

void System::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
   events.Subscribe<Engine::ComponentAddedEvent<Collider>>(*this);
   events.Subscribe<Engine::ComponentRemovedEvent<Collider>>(*this);

   updateMetric = DebugHelper::Instance()->RegisterMetric("Physics Update", [this]() -> std::string {
      return Format::FormatString("%.2fms", mUpdateClock.Average() * 1000.0);
   });

   collisionMetric = DebugHelper::Instance()->RegisterMetric("Collision Checks", [this]() -> std::string {
      return Format::FormatString("%.2fms", mCollisionClock.Average() * 1000.0);
   });
}

void System::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   const float GRAVITY = 4*9.8f;

   // Update position for all entities
   mUpdateClock.Reset();
   entities.Each<Engine::Transform, Body>([&](Engine::Entity, Engine::Transform& transform, Body& body) {
      // TODO parent transforms make my brain hurt
      assert(!transform.GetParent());

      // Gravity
      if (!body.lockPosition.y) body.velocity.y -= GRAVITY * float(dt);

      // Movement
      transform.SetLocalPosition(transform.GetLocalPosition() + float(dt) * body.velocity);
   });
   mUpdateClock.Elapsed();

   // Check for collisions, and resolve them.
   mCollisionClock.Reset();
   using EntityList = Engine::EntityManager::EntityView<Body>;
   EntityList bodies = entities.EntitiesWithComponents<Body>();
   for (EntityList::Iterator entity = bodies.begin(); entity != bodies.end(); ++entity)
   {
      // To make it easier, don't do any computations if the source is an object that can't move.
      // If a second object hits it, the resolution will be done with that object as primary.
      Engine::ComponentHandle<Body> body = (*entity).Get<Body>();
      if (body->lockPosition.x && body->lockPosition.y && body->lockPosition.z)
      {
         continue;
      }

      ColliderHandle collider = (*entity).Get<Collider>();
      if (!collider)
      {
         continue;
      }

      glm::vec3 position = (*entity).Get<Engine::Transform>()->GetAbsolutePosition();
      AABB self(position - collider->size / 2.0f, position + collider->size / 2.0f);
      uint32_t checks = 0;

      // Use the AABB Tree to find all colliding AABBs
      std::deque<AABBTree<Engine::Entity::ID>::Node> pending;
      pending.push_back(mBodies.GetRoot());
      while(!pending.empty())
      {
         AABBTree<Engine::Entity::ID>::Node node = pending.back();
         pending.pop_back();

         ++checks;
         if (node->aabb.Overlapping(self))
         {
            if (node.IsLeaf())
            {
               Engine::Entity::ID id = node.data();
               if (id == (*entity).GetID())
               {
                  continue;
               }

               Engine::Entity other = entities.GetEntity(id);
               ResolveCollision(*entity, other, dt);

               // Figure out the new hitbox.
               glm::vec3 newPosition = (*entity).Get<Engine::Transform>()->GetAbsolutePosition();
               self = AABB(newPosition - collider->size / 2.0f, newPosition + collider->size / 2.0f);
            }
            else
            {
               pending.push_back(node.left());
               pending.push_back(node.right());
            }
         }
      }
   }
   mCollisionClock.Elapsed();
}

void System::Receive(const Engine::ComponentAddedEvent<Collider>& e)
{
   Engine::ComponentHandle<Collider> handle = e.component;
   Collider* collider = handle.get();
   
   TransformHandle transform = e.entity.Get<Engine::Transform>();
   // TODO parent transforms !?
   assert(!transform->GetParent());
   glm::vec3 position = transform->GetAbsolutePosition();

   // Add to the tree
   AABB aabb(position - collider->size / 2.0f, position + collider->size / 2.0f);
   size_t index;
   if (!mEntitiesFreeList.empty())
   {
      index = mEntitiesFreeList.back();
      mEntitiesFreeList.pop_back();
   }
   else
   {
      index = mEntities.size();
      mEntities.resize(index + 1);
   }
   mEntities[index] = e.entity.GetID();
   // Store the index as a "pointer"
   mBodies.Insert(aabb, e.entity.GetID());
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
void Debug::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   if (!mActive)
   {
      return;
   }

   BIND_PROGRAM_IN_SCOPE(program);

   glm::mat4 perspective = mCamera->GetPerspective();
   glm::mat4 view = mCamera->GetView();
   program->UniformMatrix4f("uProjMatrix", perspective);
   program->UniformMatrix4f("uViewMatrix", view);

   entities.Each<Engine::Transform, Collider>([&](Engine::Entity, Engine::Transform& transform, Collider& collider) {
      glm::vec3 pos = transform.GetAbsolutePosition();
      program->UniformVector3f("uPosition", pos);
      program->UniformVector3f("uSize", collider.size);

      glDrawArrays(GL_POINTS, 0, 1);

      CHECK_GL_ERRORS();
   });
}

}; // namespace SimplePhysics

}; // namespace CubeWorld

// By Thomas Steinke

#include <Engine/Logger/Logger.h>

#include "../Event/NamedEvent.h"
#include "FlySystem.h"
#include "SimplePhysicsSystem.h"

namespace CubeWorld
{

void FlySystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   int isW = mInput->IsKeyDown(GLFW_KEY_W) ? 1 : 0;
   int isA = mInput->IsKeyDown(GLFW_KEY_A) ? 1 : 0;
   int isS = mInput->IsKeyDown(GLFW_KEY_S) ? 1 : 0;
   int isD = mInput->IsKeyDown(GLFW_KEY_D) ? 1 : 0;

   glm::vec3 flyDirection(0);
   if (isW || isA || isS || isD)
   {
      flyDirection = glm::normalize(glm::vec3(isD - isA, 0, isW - isS));
   }
   
   entities.Each<Engine::Transform, FlySpeed, SimplePhysics::Body>([&](Engine::Entity /*entity*/, Engine::Transform& transform, FlySpeed& fly, SimplePhysics::Body& body) {
      glm::vec3 dir = float(fly.speed) * glm::normalize(transform.GetFlatDirection());

      glm::vec3 forward = glm::vec3(dir.x, 0, dir.z);
      glm::vec3 right = glm::vec3(-dir.z, 0, dir.x);

      body.velocity = glm::vec3(0, body.velocity.y, 0) + forward * flyDirection.z + right * flyDirection.x;

      if (mInput->IsKeyDown(GLFW_KEY_SPACE))
      {
         body.velocity.y = 15;
      }
   });
}

}; // namespace CubeWorld

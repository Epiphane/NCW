// By Thomas Steinke

#include <Engine/Logger/Logger.h>

#include "FlySystem.h"

namespace CubeWorld
{

namespace Game
{
   
   void FlySystem::Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt)
   {
      bool isW = mInput->IsKeyDown(GLFW_KEY_W);
      bool isA = mInput->IsKeyDown(GLFW_KEY_A);
      bool isS = mInput->IsKeyDown(GLFW_KEY_S);
      bool isD = mInput->IsKeyDown(GLFW_KEY_D);
      bool isQ = mInput->IsKeyDown(GLFW_KEY_Q);
      bool isE = mInput->IsKeyDown(GLFW_KEY_E);

      if (!isW && !isA && !isS && !isD && !isQ && !isE)
      {
         return;
      }

      glm::vec3 flyDirection = float(dt) * glm::normalize(glm::vec3(isD - isA, isE - isQ, isW - isS));
      
      // TODO why can't I just put a lambda in here?
      entities.Each<Engine::Transform, FlySpeed>([&](Engine::Entity /*entity*/, Engine::Transform& transform, FlySpeed& fly) {
         glm::vec3 dir = float(fly.speed) * glm::normalize(transform.GetFlatDirection());

         glm::vec3 forward = glm::vec3(dir.x, 0, dir.z);
         glm::vec3 right = glm::vec3(-dir.z, 0, dir.x);

         transform.position += forward * flyDirection.z + right * flyDirection.x + glm::vec3(0, fly.speed * flyDirection.y, 0);
      });
   }
   
}; // namespace Game

}; // namespace CubeWorld

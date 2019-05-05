// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "../Event/NamedEvent.h"
#include "AnimationSystem.h"
#include "WalkSystem.h"
#include "SimplePhysicsSystem.h"

namespace CubeWorld
{

void WalkSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   float inputAngle = 0; // 0 means straight forward, 90 means right strafe
   bool forward = mInput->IsKeyDown(GLFW_KEY_W) && !mInput->IsKeyDown(GLFW_KEY_S);
   bool backward = mInput->IsKeyDown(GLFW_KEY_S) && !mInput->IsKeyDown(GLFW_KEY_W);
   bool left = mInput->IsKeyDown(GLFW_KEY_A) && !mInput->IsKeyDown(GLFW_KEY_D);
   bool right = mInput->IsKeyDown(GLFW_KEY_D) && !mInput->IsKeyDown(GLFW_KEY_A);
   if (backward)
   {
      if (left)
      {
         inputAngle = 135;
      }
      else if (right)
      {
         inputAngle = 225;
      }
      else {
         inputAngle = 180;
      }
   }
   else if (forward)
   {
      if (left)
      {
         inputAngle = 45;
      }
      else if (right)
      {
         inputAngle = 315;
      }
      else {
         inputAngle = 0;
      }
   }
   else if (left)
   {
      inputAngle = 90;
   }
   else if (right)
   {
      inputAngle = 270;
   }

   entities.Each<Engine::Transform, WalkDirector, WalkSpeed>([&](Engine::Entity, Engine::Transform& transform, WalkDirector& director, WalkSpeed& speed) {
      if (!forward && !backward && !left && !right)
      {
         // No input, no change.
         speed.walking = speed.running = false;
         return;
      }

      speed.walking = mInput->IsKeyDown(GLFW_KEY_LEFT_SHIFT);
      speed.running = !speed.walking;

      float directorAngle = director.director->GetYaw();
      float walkAngle = directorAngle + RADIANS(inputAngle);
      transform.SetYaw(walkAngle);

      // Make sure the director stays facing how it was (globally)
      if (director.anchor)
      {
         director.director->SetYaw(directorAngle - walkAngle);
      }
   });
   
   entities.Each<Engine::Transform, WalkSpeed, SimplePhysics::Body>([&](Engine::Entity /*entity*/, Engine::Transform& transform, WalkSpeed& walk, SimplePhysics::Body& body) {
      float goalSpeed = 0;
      if (walk.running)
      {
         goalSpeed = walk.runSpeed;
      }
      else if (walk.walking)
      {
         goalSpeed = walk.walkSpeed;
      }

      if (walk.currentSpeed < goalSpeed)
      {
         walk.currentSpeed += walk.accel * float(dt);
      }
      if (walk.currentSpeed > goalSpeed)
      {
         walk.currentSpeed -= walk.accel * float(dt);

         if (walk.currentSpeed < goalSpeed)
         {
            walk.currentSpeed = goalSpeed;
         }
      }

      glm::vec3 dir = float(walk.currentSpeed) * glm::normalize(transform.GetFlatDirection());

      body.velocity = glm::vec3(0, body.velocity.y, 0) + dir;

      if (mInput->IsKeyDown(GLFW_KEY_SPACE))
      {
         body.velocity.y = 15;
      }
   });

   entities.Each<WalkSpeed, SimplePhysics::Body, DeprecatedController>([&](Engine::Entity, WalkSpeed&, SimplePhysics::Body& body, DeprecatedController& skeleton) {
      skeleton.SetParameter("speed", std::sqrt(body.velocity.x * body.velocity.x + body.velocity.z * body.velocity.z));
   });
}

}; // namespace CubeWorld

// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "../Event/NamedEvent.h"
#include "AnimationSystem.h"
#include "WalkSystem.h"
#include "BulletPhysicsSystem.h"

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

   // BulletPhysics behavior
   entities.Each<Engine::Transform, WalkSpeed, BulletPhysics::ControlledBody>([&](Engine::Transform& transform, WalkSpeed& walk, BulletPhysics::ControlledBody& body) {

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

      glm::vec3 dir = glm::normalize(transform.GetFlatDirection());
      dir *= float(walk.currentSpeed);
      body.controller->setWalkDirection(btVector3{dir.x, 0, dir.z});

      if (mInput->IsKeyDown(GLFW_KEY_SPACE))
      {
         body.controller->jump();
      }
   });

   entities.Each<WalkSpeed, BulletPhysics::ControlledBody, AnimationController>([&](Engine::Entity, WalkSpeed&, BulletPhysics::ControlledBody& body, AnimationController& skeleton) {
      const auto& linearVelocity = body.controller->getLinearVelocity();
      float speed = std::sqrt(linearVelocity.getX() * linearVelocity.getX() + linearVelocity.getZ() * linearVelocity.getZ());
      skeleton.SetParameter("speed", speed);
   });
}

}; // namespace CubeWorld

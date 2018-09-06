// By Thomas Steinke

#include "../Main.h"

#include "CameraSystem.h"

namespace CubeWorld
{

namespace Game
{
   
   void CameraSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
   {
      if (mInput->IsMouseLocked())
      {
         double movement[2];
         double scroll[2];
         mInput->GetMouseMovement(movement);
         mInput->GetMouseScroll(scroll);
         
         entities.Each<Engine::Transform, MouseControlledCamera>([&](Engine::Entity /*entity*/, Engine::Transform& transform, MouseControlledCamera& opts) {
            transform.SetYaw(transform.GetYaw() + opts.sensitivity[0] * movement[0]);

            float newPitch = transform.GetPitch() - opts.sensitivity[1] * movement[1];
            if (newPitch < -M_PI / 2.0f + 0.01f)
            {
               newPitch = -M_PI / 2.0f + 0.01f;
            }
            if (newPitch > M_PI / 2.0f - 0.01f)
            {
               newPitch = M_PI / 2.0f - 0.01f;
            }
            transform.SetPitch(newPitch);
         });

         entities.Each<ArmCamera, MouseControlledCameraArm>([&](Engine::Entity /*entity*/, ArmCamera& camera, MouseControlledCameraArm& opts) {
            camera.distance -= opts.sensitivity * scroll[1];

            if (camera.distance < camera.minDistance)
            {
               camera.distance = camera.minDistance;
            }
            else if (camera.distance > camera.maxDistance)
            {
               camera.distance = camera.maxDistance;
            }
         });
      }
      else
      {
         entities.Each<Engine::Transform, KeyControlledCamera>([&](Engine::Entity /*entity*/, Engine::Transform& transform, KeyControlledCamera& opts) {
            int yMove = mInput->IsKeyDown(opts.keys[1]) - mInput->IsKeyDown(opts.keys[0]);
            int xMove = mInput->IsKeyDown(opts.keys[3]) - mInput->IsKeyDown(opts.keys[2]);

            transform.SetYaw(transform.GetYaw() + dt * opts.sensitivity[0] * xMove);

            float newPitch = transform.GetPitch() - dt * opts.sensitivity[1] * yMove;
            if (newPitch < -M_PI / 2.0f + 0.01f)
            {
               newPitch = -M_PI / 2.0f + 0.01f;
            }
            if (newPitch > M_PI / 2.0f - 0.01f)
            {
               newPitch = M_PI / 2.0f - 0.01f;
            }
            transform.SetPitch(newPitch);
         });

         entities.Each<ArmCamera, KeyControlledCameraArm>([&](Engine::Entity /*entity*/, ArmCamera& camera, KeyControlledCameraArm& opts) {
            int move = mInput->IsKeyDown(opts.zoomOut) - mInput->IsKeyDown(opts.zoomIn);
            camera.distance += dt * opts.sensitivity * move;

            if (camera.distance < camera.minDistance)
            {
               camera.distance = camera.minDistance;
            }
            else if (camera.distance > camera.maxDistance)
            {
               camera.distance = camera.maxDistance;
            }
         });
      }
   }
   
}; // namespace Game

}; // namespace CubeWorld

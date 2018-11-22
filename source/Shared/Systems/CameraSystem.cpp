// By Thomas Steinke

#include <Engine/Logger/Logger.h>

#include "../Main.h"

#include "CameraSystem.h"

namespace CubeWorld
{

void CameraSystem::Receive(const Engine::ComponentAddedEvent<MouseDragCamera>& evt)
{
   mDraggables.push_back(evt.component);
}

void CameraSystem::Receive(const MouseDownEvent& evt)
{
   glm::tvec2<double> mouse = mInput->GetMousePosition();
   if (mouse.x < 0 || mouse.x > 1 || mouse.y < 0 || mouse.y > 1)
   {
      return;
   }

   for (Engine::ComponentHandle<MouseDragCamera>& draggable : mDraggables)
   {
      if (evt.button == draggable->button)
      {
         draggable->engaged = true;
      }
   }
}

void CameraSystem::Receive(const MouseUpEvent& evt)
{
   for (Engine::ComponentHandle<MouseDragCamera>& draggable : mDraggables)
   {
      if (evt.button == draggable->button)
      {
         draggable->engaged = false;
      }
   }
}

void CameraSystem::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
   events.Subscribe<Engine::ComponentAddedEvent<MouseDragCamera>>(*this);
   events.Subscribe<MouseDownEvent>(*this);
   events.Subscribe<MouseUpEvent>(*this);
}

void CameraSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA dt)
{
   glm::tvec2<double> scroll = mInput->GetMouseScroll();
   glm::tvec2<double> mouse = mInput->GetMousePosition();

   if (mInput->IsMouseLocked())
   {
      glm::tvec2<double> movement = mInput->GetMouseMovement();
      
      entities.Each<Engine::Transform, MouseControlledCamera>([&](Engine::Entity /*entity*/, Engine::Transform& transform, MouseControlledCamera& opts) {
         transform.SetYaw(transform.GetYaw() - float(opts.sensitivity[0] * movement.x));

         float newPitch = transform.GetPitch() - float(opts.sensitivity[1] * movement.y);
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
   }
   else
   {
      entities.Each<Engine::Transform, KeyControlledCamera>([&](Engine::Entity /*entity*/, Engine::Transform& transform, KeyControlledCamera& opts) {
         int yMove = mInput->IsKeyDown(opts.keys[1]) - mInput->IsKeyDown(opts.keys[0]);
         int xMove = mInput->IsKeyDown(opts.keys[3]) - mInput->IsKeyDown(opts.keys[2]);

         transform.SetYaw(transform.GetYaw() - float(dt * opts.sensitivity[0] * xMove));

         float newPitch = transform.GetPitch() + float(dt * opts.sensitivity[1] * yMove);
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
   }

   entities.Each<Engine::Transform, MouseDragCamera>([&](Engine::Entity /*entity*/, Engine::Transform& transform, MouseDragCamera& opts) {
      if (opts.engaged)
      {
         glm::tvec2<double> movement = mInput->GetMouseMovement();
         transform.SetYaw(transform.GetYaw() - float(opts.sensitivity[0] * movement.x));

         float newPitch = transform.GetPitch() - float(opts.sensitivity[1] * movement.y);
         if (newPitch < -M_PI / 2.0f + 0.01f)
         {
            newPitch = -M_PI / 2.0f + 0.01f;
         }
         if (newPitch > M_PI / 2.0f - 0.01f)
         {
            newPitch = M_PI / 2.0f - 0.01f;
         }
         transform.SetPitch(newPitch);
      }
   });

   if (mouse.x >= 0 && mouse.x <= 1 && mouse.y >= 0 && mouse.y <= 1)
   {
      entities.Each<ArmCamera, MouseControlledCameraArm>([&](Engine::Entity /*entity*/, ArmCamera& camera, MouseControlledCameraArm& opts) {
         camera.distance -= float(opts.sensitivity * scroll.y);

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

   entities.Each<ArmCamera, KeyControlledCameraArm>([&](Engine::Entity /*entity*/, ArmCamera& camera, KeyControlledCameraArm& opts) {
      int move = mInput->IsKeyDown(opts.zoomOut) - mInput->IsKeyDown(opts.zoomIn);
      camera.distance += float(dt * opts.sensitivity * move);

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

}; // namespace CubeWorld

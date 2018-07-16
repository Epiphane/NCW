// By Thomas Steinke

#include <Game/Main.h>
#include <Game/Components/RenderCamera.h>

#include "CameraSystem.h"

namespace CubeWorld
{

namespace Game
{
   
   void CameraSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
   {
      double movement[2];
      mInput->GetMouse(nullptr, movement);
      
      // TODO why can't I just put a lambda in here?
      entities.Each<Engine::Transform, MouseControlledCamera>([&](Engine::Entity /*entity*/, Engine::Transform& transform, MouseControlledCamera& opts) {
         transform.SetYaw(transform.GetYaw() + opts.sensitivity[0] * movement[0]);

         float newPitch = transform.GetPitch() + opts.sensitivity[1] * movement[1];
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
   
}; // namespace Game

}; // namespace CubeWorld

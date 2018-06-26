// By Thomas Steinke

#include <Game/Main.h>
#include <Game/Components/RenderCamera.h>

#include "CameraSystem.h"

namespace CubeWorld
{

namespace Game
{
   
   void CameraSystem::Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt)
   {
      double movement[2];
      mInput->GetMouse(nullptr, movement);
      
      // TODO why can't I just put a lambda in here?
      std::function<void(Engine::Entity, Engine::Transform&, MouseControlledCamera&)> fn = [&](Engine::Entity /*entity*/, Engine::Transform& transform, MouseControlledCamera& opts) {
         transform.SetYaw(transform.GetYaw() + opts.sensitivity[0] * movement[0]);

         float newPitch = transform.GetPitch() + opts.sensitivity[1] * movement[1];
         if (newPitch < -M_PI / 2)
         {
            newPitch = -M_PI / 2;
         }
         if (newPitch > M_PI / 2)
         {
            newPitch = M_PI / 2;
         }
         transform.SetPitch(newPitch);
      };
      entities.Each<Engine::Transform, MouseControlledCamera>(fn);
   }
   
}; // namespace Game

}; // namespace CubeWorld

// By Thomas Steinke

#include "CameraSystem.h"

#include <Game/Components/RenderCamera.h>

namespace CubeWorld
{

namespace Game
{
   
   void CameraSystem::Update(Engine::EntityManager& entities/*, EventManager& events*/, TIMEDELTA dt)
   {
      double movement[2];
      mInput->GetMouse(nullptr, movement);
      
      // TODO why can't I just put a lambda in here?
      std::function<void(Engine::Entity, Component::RenderCamera&, MouseControlledCamera&)> fn = [&](Engine::Entity /*entity*/, Component::RenderCamera& camera, MouseControlledCamera& opts) {
         camera.SetYaw(camera.yaw + opts.sensitivity[0] * movement[0]);
         camera.SetPitch(camera.pitch + opts.sensitivity[1] * movement[1]);
      };
      entities.Each<Component::RenderCamera, MouseControlledCamera>(fn);
   }
   
}; // namespace Game

}; // namespace CubeWorld

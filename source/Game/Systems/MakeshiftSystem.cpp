// By Thomas Steinke

#include <Engine/Logger/Logger.h>

#include "MakeshiftSystem.h"

namespace CubeWorld
{

namespace Game
{
   
   void MakeshiftSystem::Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt)
   {
      entities.Each<Makeshift>([&](Engine::Entity /*entity*/, Makeshift& custom) {
         custom.callback(entities, events, dt);
      });
   }
   
}; // namespace Game

}; // namespace CubeWorld

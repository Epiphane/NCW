// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "MakeshiftSystem.h"

namespace CubeWorld
{

void MakeshiftSystem::Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt)
{
   entities.Each<Makeshift>([&](Makeshift& custom) {
      custom.callback(entities, events, dt);
   });
}

}; // namespace CubeWorld

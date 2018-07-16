// By Thomas Steinke

#include "EventManager.h"

namespace CubeWorld
{

namespace Engine
{

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
   for (auto ring : mEventRings)
   {
      assert(ring->refs == 1);
      ring->Unlink();
   }
}

}; // namespace Engine

}; // namespace CubeWorld

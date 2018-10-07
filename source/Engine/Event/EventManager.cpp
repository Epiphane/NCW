// By Thomas Steinke

#include "EventManager.h"

namespace CubeWorld
{

namespace Engine
{

EventManager::EventManager()
{
   mDownstreamConnections = std::make_unique<ManagerLink>(this, nullptr);
   mDownstreamConnections->next = mDownstreamConnections.get();
   mDownstreamConnections->prev = mDownstreamConnections.get();
}

EventManager::~EventManager()
{
   for (auto ring : mEventRings)
   {
      if (ring != nullptr)
      {
         assert(ring->refs == 1);
         ring->Unlink();
      }
   }
}

void EventManager::RemoveLink(ManagerLink* link)
{
   if (link->next != nullptr)
   {
      link->next->prev = link->prev;
   }

   if (link->prev != nullptr)
   {
      link->prev->next = link->next;
   }

   link->next = nullptr;
   link->prev = nullptr;
}

std::unique_ptr<EventManager::ManagerLink> EventManager::AddChild(EventManager* child)
{
   std::unique_ptr<ManagerLink> link = std::make_unique<ManagerLink>(this, child);

   link->next = mDownstreamConnections.get();
   link->prev = mDownstreamConnections->prev;
   link->prev->next = link.get();
   link->next->prev = link.get();

   return std::move(link);
}

}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <unordered_map>
#include <utility>

#include "Event.h"
#include "EventLink.h"

namespace CubeWorld
{

namespace Engine
{

class EventManager;

class BaseReceiver {
public:
   virtual ~BaseReceiver()
   {
      for (auto connection : mConnections)
      {
         if (connection.second != nullptr)
         {
            connection.second->Unlink();
         }
      }
   }

private:
   friend class EventManager;

   std::unordered_map<BaseEvent::Family, BaseEventLink*> mConnections;
};

template <typename Derived>
class Receiver : public BaseReceiver {
public:
   virtual ~Receiver() {}
};

}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <functional>
#include <vector>

#include "Event.h"
#include "EventLink.h"
#include "Receiver.h"

namespace CubeWorld
{

namespace Engine
{

//
// At its core, EventManager serves as the de facto way of getting information from one place to another.
// For example, if you want to do something each time an Entity is created, a specific component is added
// an entity, or when the player clicks their mouse, you'll want to make use of the EventManager.
//
// Generally speaking, each State has one EventManager, which is uses for piping all this communication.
// The manager is provided to the Update method of all Systems, so as part of the update loop you have
// the opportunity to call events.Emit<WhateverEvent>(args, to, event, constructor);
//
// Whatever you'd like to receive this event (the state, another system, anything) just has to implement
// Receive<WhateverEvent> (see below) and be registered at one point. For example, a state might subscribe
// like this:
//    mEvents.Subscribe<WhateverEvent>(*this);
//
class EventManager {
public:
   EventManager();
   virtual ~EventManager();

   //
   // Subscribe to events of type E.
   //
   // Receivers must be a subclass of Receiver and implement a Receive() method for the event type.
   // For example:
   //
   // struct CollisionReceiver : public Receiver<CollisionReceiver> {
   //    void Receive(const Collision& event)
   //    { ... }
   // };
   //
   // CollisionReceiver receiver;
   // em.Subscribe<Collision>(receiver);
   //
   template <typename E, typename Receiver>
   void Subscribe(Receiver& receiver)
   {
      void (Receiver::*Receive)(const E&) = &Receiver::Receive;
      auto callback = std::bind(Receive, &receiver, std::placeholders::_1);
      EventLink<E>* link = new EventLink<E>(callback);
      EventLink<E>* ring = GetEventRing<E>();
      link->prev = ring->prev;
      link->next = ring;
      ring->prev->next = link;
      ring->prev = link;

      BaseReceiver& base = receiver;
      base.mConnections.emplace(Event<E>::GetFamily(), link);
   }

   template <typename E, typename Receiver>
   void Unsubscribe(Receiver& receiver)
   {
      BaseReceiver& base = receiver;
      const BaseEvent::Family family = Event<E>::GetFamily();

      assert(base.mConnections.find(family) != base.mConnections.end());
      auto link = base.mConnections[family];
      link->Unlink();

      base.mConnections.erase(family);
   }

   template <typename E>
   void Emit(const E& evt)
   {
      EventLink<E>* ring = GetEventRing<E>();
      EventLink<E>* link = ring;
      link->IncRef();
      do
      {
         if (link->callback)
         {
            link->callback(evt);
         }
         EventLink<E>* old = link;
         link = link->next;
         link->IncRef();
         old->DecRef();
      }
      while (link != ring);
      link->DecRef();
   }

   template <typename E, typename ... Args>
   void Emit(Args && ... args)
   {
      Emit(E(std::forward<Args>(args)...));
   }

private:
   template <typename E>
   EventLink<E>* GetEventRing()
   {
      const BaseEvent::Family family = Event<E>::GetFamily();
      if (family >= mEventRings.size())
      {
         mEventRings.resize(family + 1, nullptr);
      }

      EventLink<E>* ring = static_cast<EventLink<E>*>(mEventRings[family]);
      if (ring == nullptr)
      {
         ring = new EventLink<E>(nullptr);
         ring->next = ring->prev = ring;
         mEventRings[family] = ring;
      }
      return ring;
   }

   std::vector<BaseEventLink*> mEventRings;
};

}; // namespace Engine

}; // namespace CubeWorld

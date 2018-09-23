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
   void EmitInternal(const E& evt)
   {
      {
         // Send the event to any direct subscribers
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
         } while (link != ring);
         link->DecRef();
      }

      {
         // Then send it to any downstream managers
         ManagerLink* ring = mDownstreamConnections.get();
         ManagerLink* link = ring;
         do
         {
            if (link->target != nullptr)
            {
               link->target->EmitInternal<E>(evt);
            }
            link = link->next;
         } while (link != ring);
      }
   }

   template <typename E>
   void Emit(const E& evt)
   {
      if (!mParent)
      {
         EmitInternal<E>(evt);
      }
      else
      {
         mParent->source->Emit<E>(evt);
      }
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

private:
   //
   // All the following code pertains to multiple EventManagers coexisting, and
   // more importantly, communicating with each other. Any EventManager can
   // SubscribeTo(another), which will cause it to propagate all events it receives
   // up to that manager. There isn't much in place to prevent circular dependencies,
   // so good luck - be smart.
   //

   // Downstream managers are stored as a doubly-linked list, to allow for easy insertion and removal.
   struct ManagerLink {
      ManagerLink(EventManager* source, EventManager* target)
         : source(source)
         , target(target)
         , next(nullptr)
         , prev(nullptr)
      {};
      ~ManagerLink()
      {
         source->RemoveLink(this);
      }

   private:
      friend class EventManager;
      EventManager* source;
      EventManager* target;

      ManagerLink* next;
      ManagerLink* prev;
   };

   void RemoveLink(ManagerLink* link);
   
public:
   void SetParent(EventManager* parent)
   {
      mParent = parent->AddChild(this);
   }

private:
   std::unique_ptr<ManagerLink> AddChild(EventManager* child);

   //
   // Adventure of the unique pointers! An EventManager holds ownership of:
   // - The head of its downstream event ring
   // - All the links it received when subscribing to another EventManager
   //
   // When an EventManager goes away, the following happens: the head of its
   // event ring is collected, which closes up to convert all existing links
   // in the ring (which are owned by downstream EventManager) into a complete
   // ring that is not referenced by anything. Then, its upstream link is
   // deallocated, removing them from the ring it might be a part of.
   //
   // As a result of this method, a Unsubscribe or Remove endpoint isn't
   // necessary. Just release your handle to the ManagerLink and it will
   // be cleaned up.
   //
   std::unique_ptr<ManagerLink> mDownstreamConnections;
   std::unique_ptr<ManagerLink> mParent;
};

}; // namespace Engine

}; // namespace CubeWorld

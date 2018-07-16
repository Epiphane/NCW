// By Thomas Steinke

#pragma once

#include <cassert>
#include <functional>

namespace CubeWorld
{

namespace Engine
{

// Use a doubly-linked circular list for maintaining all the callbacks for a specific event.
struct BaseEventLink {
   int32_t refs;
   explicit BaseEventLink() : refs(1) {};
   ~BaseEventLink() { assert(refs == 0); }

   // Manage refs count
   void IncRef()
   {
      refs += 1;
      assert(refs > 0);
   }
   void DecRef()
   {
      refs -= 1;
      if (!refs)
      {
         delete this;
      }
      else
      {
         assert(refs > 0);
      }
   }

   virtual void Unlink() = 0;
};

// EventLink is an event-type-specific implementation of BaseEventLink
template <typename E>
struct EventLink : public BaseEventLink {
   EventLink* next;
   EventLink* prev;
   using Callback = std::function<void(const E&)>;

   Callback callback;
   explicit EventLink(const Callback& callback) : BaseEventLink(), next(nullptr), prev(nullptr), callback(callback) {};
   void Unlink() override
   {
      callback = nullptr;
      if (next)
      {
         next->prev = prev;
      }
      if (prev)
      {
         prev->next = next;
      }
      DecRef();
   }
};

}; // namespace Engine

}; // namespace CubeWorld

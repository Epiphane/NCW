// By Thomas Steinke

#pragma once

#include <string>

#include "Event.h"

namespace CubeWorld
{

struct MouseDragEvent : public Engine::Event<MouseDragEvent> {
   explicit MouseDragEvent(int button, double x, double y)
      : button{button}
      , x{x}
      , y{y}
   {};
   virtual ~MouseDragEvent() {}

   int button;
   double x, y;
};

struct MouseDownEvent : public Engine::Event<MouseDownEvent> {
   explicit MouseDownEvent(int button, double x, double y)
      : button{button}
      , x{x}
      , y{y}
   {};
   virtual ~MouseDownEvent() {}

   int button;
   double x, y;
};

struct MouseUpEvent : public Engine::Event<MouseUpEvent> {
   explicit MouseUpEvent(int button, double x, double y)
      : button{button}
      , x{x}
      , y{y}
   {};
   virtual ~MouseUpEvent() {}

   int button;
   double x, y;
};

struct MouseClickEvent : public Engine::Event<MouseClickEvent> {
   explicit MouseClickEvent(int button, double x, double y)
      : button{button}
      , x{x}
      , y{y}
   {};
   virtual ~MouseClickEvent() {}

   int button;
   double x, y;
};

}; // namespace CubeWorld

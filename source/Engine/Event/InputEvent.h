// By Thomas Steinke

#pragma once

#include <string>

#include "Event.h"

namespace CubeWorld
{

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

struct MouseMoveEvent : public Engine::Event<MouseMoveEvent> {
   explicit MouseMoveEvent(double x, double y)
      : x{x}
      , y{y}
   {};
   virtual ~MouseMoveEvent() {}

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

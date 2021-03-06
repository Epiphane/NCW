// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/Event/Event.h>

namespace CubeWorld
{

struct NamedEvent : public Engine::Event<NamedEvent> {
   explicit NamedEvent(std::string name) : name(name) {}
   virtual ~NamedEvent() {}

   std::string name;
};

}; // namespace CubeWorld

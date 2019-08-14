// By Thomas Steinke

#pragma once

#include <cstdint>
#include <sys/types.h>

namespace CubeWorld
{

namespace Engine
{

class BaseEvent {
public:
   typedef size_t Family;

protected:
   // This gets incremented with each different Event class
   static Family sNumFamilies;
};

template <typename Derived>
class Event : public BaseEvent {
public:
   // Used internally for registration. Check out explanation in Component.h
   static Family GetFamily()
   {
      static Family family = sNumFamilies++;
      return family;
   }
};

}; // namespace Engine

}; // namespace CubeWorld

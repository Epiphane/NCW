// By Thomas Steinke

#pragma once

#include <cstdint>
#include <string>
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

#define _START_NAMED_EVENT(name) \
    struct name : public CubeWorld::Engine::Event<name> \
    {

#define _EXPAND(x) x
#define _NAMED_EVENT_ARGS_EXPAND(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N

/*
#define _NAMED_EVENT_CONSTRUCTOR_ARGS1(t, m) const t& m
#define _NAMED_EVENT_CONSTRUCTOR_ARGS2(t1, m1, t, m) \
    _NAMED_EVENT_CONSTRUCTOR_ARGS1(t1, m1), \
    _NAMED_EVENT_CONSTRUCTOR_ARGS1(t, m)
#define _NAMED_EVENT_CONSTRUCTOR_ARGS3(t1, m1, t2, m2, t, m) \
    _NAMED_EVENT_CONSTRUCTOR_ARGS2(t1, m1, t2, m2), \
    _NAMED_EVENT_CONSTRUCTOR_ARGS1(t, m)
#define _NAMED_EVENT_CONSTRUCTOR_ARGS4(t1, m1, t2, m2, t3, m3, t, m) \
    _NAMED_EVENT_CONSTRUCTOR_ARGS3(t1, m1, t2, m2, t3, m3), \
    _NAMED_EVENT_CONSTRUCTOR_ARGS1(t, m)
#define _NAMED_EVENT_CONSTRUCTOR_ARGS_OVERLOAD(...) _EXPAND(_NAMED_EVENT_ARGS_EXPAND(__VA_ARGS__,\
    _NAMED_EVENT_CONSTRUCTOR_ARGS4, e4,\
    _NAMED_EVENT_CONSTRUCTOR_ARGS3, e3,\
    _NAMED_EVENT_CONSTRUCTOR_ARGS2, e2,\
    _NAMED_EVENT_CONSTRUCTOR_ARGS1, e1))
#define _NAMED_EVENT_CONSTRUCTOR_ARGS(...) \
    _EXPAND(_NAMED_EVENT_CONSTRUCTOR_ARGS_OVERLOAD(__VA_ARGS__)(__VA_ARGS__))

#define _NAMED_EVENT_CONSTRUCTOR_ASSIGN1(m1) abc(def)
#define _NAMED_EVENT_CONSTRUCTOR_ASSIGN2(m1, m2) \
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN1(m1), \
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN1(m2)
#define _NAMED_EVENT_CONSTRUCTOR_ASSIGN3(m1, m2, m3) \
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN2(m1, m2), \
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN1(m3)
#define _NAMED_EVENT_CONSTRUCTOR_ASSIGN4(m1, m2, m3, m4) \
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN3(m1, m2, m3), \
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN1(m4)
#define _NAMED_EVENT_CONSTRUCTOR_ASSIGN_OVERLOAD(...) _EXPAND(_NAMED_EVENT_ARGS_EXPAND(__VA_ARGS__,\
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN4,\
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN3,\
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN2,\
    _NAMED_EVENT_CONSTRUCTOR_ASSIGN1))
#define _NAMED_EVENT_CONSTRUCTOR_ASSIGN(...) \
    _EXPAND(_NAMED_EVENT_CONSTRUCTOR_ASSIGN_OVERLOAD(__VA_ARGS__))

_NAMED_EVENT_CONSTRUCTOR_ASSIGN(myMemb)
*/

#define DEFINE_NAMED_EVENT(name) \
    _START_NAMED_EVENT(name) \
    };

#define DEFINE_NAMED_DATA_EVENT1(name, t1, m1) \
    _START_NAMED_EVENT(name) \
        name(const t1& m1) : m1(m1) {}; \
        t1 m1; \
    };

#define DEFINE_NAMED_DATA_EVENT2(name, t1, m1, t2, m2) \
    _START_NAMED_EVENT(name) \
        name(const t1& m1, const t2& m2) \
            : m1(m1) \
            , m2(m2) \
        {}; \
        t1 m1; \
        t2 m2; \
    };

#define _NAMED_EVENT_CONSTRUCTOR_ASSIGN_OVERLOAD(...) _EXPAND(_NAMED_EVENT_ARGS_EXPAND(__VA_ARGS__,\
    e8, e7, e6, e5,\
    DEFINE_NAMED_DATA_EVENT2, e2,\
    DEFINE_NAMED_DATA_EVENT1))
#define DEFINE_NAMED_DATA_EVENT(name, ...) \
    _EXPAND(_NAMED_EVENT_CONSTRUCTOR_ASSIGN_OVERLOAD(__VA_ARGS__)(name, __VA_ARGS__))

}; // namespace Engine

}; // namespace CubeWorld

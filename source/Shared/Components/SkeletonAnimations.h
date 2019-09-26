// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <map>
#include <vector>

#include <RGBBinding/BindingProperty.h>
#include <RGBBinding/BindingPropertyMeta.h>
#include <Engine/Entity/ComponentHandle.h>
#include "Skeleton.h"

namespace CubeWorld
{

struct SkeletonAnimations : Engine::Component<SkeletonAnimations>  {
   // Types
   struct Keyframe {
      double time = 0.0;
      std::map<std::string, glm::vec3> positions;
      std::map<std::string, glm::vec3> rotations;
      std::map<std::string, glm::vec3> scales;
   };

   struct Transition {
      struct Trigger {
         enum {
            GreaterThan,
            LessThan,
            Bool
         } type;
         std::string parameter;
         union {
            double doubleVal;
            bool boolVal;
         };
      };

      std::string destination;
      double time;
      std::vector<Trigger> triggers;
   };

   struct ParticleEffect {
      std::string name;
      std::string bone;
      double start;
      double end;
      BindingProperty modifications;
   };

   struct Event {
      enum class Type {
         Unknown,
         Strike,
      };

      Type type = Type::Unknown;
      double start;
      double end;
      BindingProperty properties;
   };

   struct State {
      std::string name;
      std::string next;
      std::string stance;

      double length = 0.0;
      std::vector<Keyframe> keyframes;

      std::vector<Transition> transitions;
      std::vector<ParticleEffect> particles;
      std::vector<Event> events;
   };

public:
   SkeletonAnimations();

   void Reset();

   // Load all animations from disk associated with the specified entity.
   SkeletonAnimations(const std::string& entity);
   void Load(const std::string& entity);

   // Load all animations from a BindingProperty. data must be a collection of objects.
   SkeletonAnimations(const std::string& entity, const BindingProperty& data);
   void Load(const std::string& entity, const BindingProperty& data);

   // Serialize all animations into a BindingProperty each.
   // Returns a collection.
   BindingProperty Serialize();

public:
   // Data
   std::string entity;
   std::map<std::string, State> states;
};

}; // namespace CubeWorld

namespace meta
{

using CubeWorld::SkeletonAnimations;

template<>
inline auto registerMembers<SkeletonAnimations::Keyframe>()
{
   return members(
      member("time", &SkeletonAnimations::Keyframe::time),
      member("positions", &SkeletonAnimations::Keyframe::positions),
      member("rotations", &SkeletonAnimations::Keyframe::rotations),
      member("scales", &SkeletonAnimations::Keyframe::scales)
   );
}

// TODO
template<>
inline auto registerMembers<SkeletonAnimations::Transition::Trigger>()
{
   return members(
      member("parameter", &SkeletonAnimations::Transition::Trigger::parameter),
      member("bool", &SkeletonAnimations::Transition::Trigger::boolVal)
   );
}

template<>
inline auto registerMembers<SkeletonAnimations::Transition>()
{
   return members(
      member("to", &SkeletonAnimations::Transition::destination),
      member("time", &SkeletonAnimations::Transition::time),
      member("triggers", &SkeletonAnimations::Transition::triggers)
   );
}

template<>
inline auto registerMembers<SkeletonAnimations::ParticleEffect>()
{
   return members(
      member("name", &SkeletonAnimations::ParticleEffect::name),
      member("bone", &SkeletonAnimations::ParticleEffect::bone),
      member("start", &SkeletonAnimations::ParticleEffect::start),
      member("end", &SkeletonAnimations::ParticleEffect::end),
      member("mods", &SkeletonAnimations::ParticleEffect::modifications)
   );
}

template<>
inline auto registerMembers<SkeletonAnimations::Event>()
{
   return members(
      //member("type", &SkeletonAnimations::Event::type),
      member("start", &SkeletonAnimations::Event::start),
      member("end", &SkeletonAnimations::Event::end),
      member("properties", &SkeletonAnimations::Event::properties)
   );
}

template<>
inline auto registerMembers<SkeletonAnimations::State>()
{
   return members(
      member("name", &SkeletonAnimations::State::name),
      member("next", &SkeletonAnimations::State::next),
      member("stance", &SkeletonAnimations::State::stance),
      member("length", &SkeletonAnimations::State::length),
      member("keyframes", &SkeletonAnimations::State::keyframes),
      member("particles", &SkeletonAnimations::State::particles),
      member("events", &SkeletonAnimations::State::events),
      member("transitions", &SkeletonAnimations::State::transitions)
   );
}

template<>
inline auto registerMembers<SkeletonAnimations>()
{
   return members(
      member("states", &SkeletonAnimations::states)
   );
}

}; // namespace meta

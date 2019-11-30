// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <map>
#include <vector>

#include <RGBBinding/BindingProperty.h>
#include <RGBBinding/BindingPropertyMeta.h>
#include <RGBMeta/Value.h>
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
         enum class Type {
            GreaterThan,
            LessThan,
            Bool
         };

         Type type;
         std::string parameter;
         union {
            double doubleVal;
            bool boolVal;
         };

         const inline Type& GetType() const;
         inline void SetType(const Type& type);

         const inline bool IsGreaterThan() const;
         const inline bool IsLessThan() const;
         const inline bool IsBool() const;
         const inline double& GetGreaterThan() const;
         const inline double& GetLessThan() const;
         const inline bool& GetBool() const;
         inline void SetGreaterThan(const double& value);
         inline void SetLessThan(const double& value);
         inline void SetBool(const bool& value);
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

      struct StrikeConfig
      {
         std::string bone;
         glm::vec3 offset = {0, 0, 0};
         glm::vec3 size = {0, 0, 0};
      };

      Type type = Type::Unknown;
      double start;
      double end;
      StrikeConfig strike;

      const inline Type& GetType() const;
      inline void SetType(const Type& type);

      const inline bool IsStrike() const;
      const inline StrikeConfig& GetStrike() const;
      inline void SetStrike(const StrikeConfig& value);
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

template<>
inline auto registerValues<SkeletonAnimations::Transition::Trigger::Type>()
{
   return values(
      value("gte", SkeletonAnimations::Transition::Trigger::Type::GreaterThan),
      value("lt", SkeletonAnimations::Transition::Trigger::Type::LessThan),
      value("bool", SkeletonAnimations::Transition::Trigger::Type::Bool)
   );
}

template<>
inline auto registerMembers<SkeletonAnimations::Transition::Trigger>()
{
   return members(
      member("parameter", &SkeletonAnimations::Transition::Trigger::parameter),
      member("type", &SkeletonAnimations::Transition::Trigger::GetType, &SkeletonAnimations::Transition::Trigger::SetType),
      member("gte", &SkeletonAnimations::Transition::Trigger::GetGreaterThan, &SkeletonAnimations::Transition::Trigger::SetGreaterThan, &SkeletonAnimations::Transition::Trigger::IsGreaterThan),
      member("lt", &SkeletonAnimations::Transition::Trigger::GetLessThan, &SkeletonAnimations::Transition::Trigger::SetLessThan, &SkeletonAnimations::Transition::Trigger::IsLessThan),
      member("bool", &SkeletonAnimations::Transition::Trigger::GetBool, &SkeletonAnimations::Transition::Trigger::SetBool, &SkeletonAnimations::Transition::Trigger::IsBool)
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
inline auto registerValues<SkeletonAnimations::Event::Type>()
{
   return values(
      value("unknown", SkeletonAnimations::Event::Type::Unknown),
      value("strike", SkeletonAnimations::Event::Type::Strike)
   );
}

template<>
inline auto registerMembers<SkeletonAnimations::Event::StrikeConfig>()
{
   return members(
      member("bone", &SkeletonAnimations::Event::StrikeConfig::bone),
      member("offset", &SkeletonAnimations::Event::StrikeConfig::offset),
      member("size", &SkeletonAnimations::Event::StrikeConfig::size)
   );
}

template<>
inline auto registerMembers<SkeletonAnimations::Event>()
{
   return members(
      member("type", &SkeletonAnimations::Event::type),
      member("start", &SkeletonAnimations::Event::start),
      member("end", &SkeletonAnimations::Event::end),
      member("strike", &SkeletonAnimations::Event::GetStrike, &SkeletonAnimations::Event::SetStrike, &SkeletonAnimations::Event::IsStrike)
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

namespace CubeWorld
{

const inline SkeletonAnimations::Transition::Trigger::Type& SkeletonAnimations::Transition::Trigger::GetType() const
{
   return type;
}

inline void  SkeletonAnimations::Transition::Trigger::SetType(const Type& t)
{
   if (type == t) { return; }
   type = t;
   if (type == Type::GreaterThan || type == Type::LessThan)
   {
      doubleVal = 0;
   }
   else if (type == Type::Bool)
   {
      boolVal = false;
   }
}

const inline bool SkeletonAnimations::Transition::Trigger::IsGreaterThan() const
{
   return type == Type::GreaterThan;
}

const inline bool SkeletonAnimations::Transition::Trigger::IsLessThan() const
{
   return type == Type::LessThan;
}

const inline bool SkeletonAnimations::Transition::Trigger::IsBool() const
{
   return type == Type::Bool;
}

const inline double& SkeletonAnimations::Transition::Trigger::GetGreaterThan() const
{
   return doubleVal;
}

const inline double& SkeletonAnimations::Transition::Trigger::GetLessThan() const
{
   return doubleVal;
}

const inline bool& SkeletonAnimations::Transition::Trigger::GetBool() const
{
   return boolVal;
}

inline void SkeletonAnimations::Transition::Trigger::SetGreaterThan(const double& value)
{
   type = Type::GreaterThan;
   doubleVal = value;
}

inline void SkeletonAnimations::Transition::Trigger::SetLessThan(const double& value)
{
   type = Type::LessThan;
   doubleVal = value;
}

inline void SkeletonAnimations::Transition::Trigger::SetBool(const bool& value)
{
   type = Type::Bool;
   boolVal = value;
}

const inline SkeletonAnimations::Event::Type& SkeletonAnimations::Event::GetType() const
{
   return type;
}

inline void SkeletonAnimations::Event::SetType(const Type& t)
{
   type = t;
}

const inline bool SkeletonAnimations::Event::IsStrike() const
{
   return type == Type::Strike;
}

const inline SkeletonAnimations::Event::StrikeConfig& SkeletonAnimations::Event::GetStrike() const
{
   return strike;
}

inline void SkeletonAnimations::Event::SetStrike(const SkeletonAnimations::Event::StrikeConfig& value)
{
   type = Type::Strike;
   strike = value;
}

}; // namespace CubeWorld

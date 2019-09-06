// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <map>
#include <vector>

#include <RGBBinding/BindingProperty.h>
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

   struct State {
      std::string name;
      std::string next;
      std::string stance;

      double length;
      std::vector<Keyframe> keyframes;
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
   std::map<std::string, std::vector<Transition>> transitions;
   std::map<std::string, std::vector<ParticleEffect>> effects;
   std::map<std::string, std::vector<Event>> events;
};

}; // namespace CubeWorld

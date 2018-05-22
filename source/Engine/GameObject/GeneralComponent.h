// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/Core/State.h>
#include <Engine/Core/Input.h>

namespace CubeWorld
{

namespace Engine
{

namespace Component
{

typedef uint32_t ComponentType;
const ComponentType Camera = 1;
const ComponentType Physics = 2;
const ComponentType Behavior = 4;
const ComponentType MAX_TYPE = 8;

class GeneralComponent {
public:
   GeneralComponent(ComponentType type) : mType(type) {}

   virtual void Update(const State* world, const Input::InputManager* input, double dt, GameObject* gameObject) = 0;

   ComponentType GetType() { return mType; }
   bool IsType(ComponentType type) { return (mType & type) != 0; }

   ComponentType mType;
};

}; // namespace Component

}; // namespace Engine

}; // namespace CubeWorld

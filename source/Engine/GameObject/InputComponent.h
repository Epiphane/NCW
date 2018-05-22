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

class InputComponent {
public:
   virtual void Update(const State* world, const Input::InputManager* input, GameObject* gameObject) = 0;
};

}; // namespace Component

}; // namespace Engine

}; // namespace CubeWorld

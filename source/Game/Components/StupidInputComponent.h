// By Thomas Steinke

#pragma once

#include <Engine/GameObject/GameObject.h>
#include <Engine/GameObject/InputComponent.h>

namespace CubeWorld
{

namespace Game
{

namespace Component
{

class StupidInputComponent : public Engine::Component::InputComponent {
public:
   struct Options
   {
      //
      // Lateral (x and z) speed
      //
      double lateralSpeed = 1.0;

      //
      // Vertical speed
      //
      double verticalSpeed = 1.0;
   };

   StupidInputComponent(const Options& options) : mOptions(options) {};

   void Update(const Engine::State* world, const Engine::Input::InputManager* input, Engine::GameObject* object) override;

   Options mOptions;
};

}; // namespace Component

}; // namespace Game

}; // namespace CubeWorld

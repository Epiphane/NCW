// By Thomas Steinke

#pragma once

#include <queue>

#include "../Core/Input.h"
#include "../Event/InputEvent.h"
#include "System.h"

namespace CubeWorld
{

namespace Engine
{

class InputEventSystem : public Engine::System<InputEventSystem> {
public:
   InputEventSystem(Engine::Input::InputManager* input)
      : mEvents{}
      , mInput(input)
   {}
   ~InputEventSystem() {}
   
   void Configure(EntityManager& entities, EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
   
private:
   void MouseDown(int button, double x, double y);
   void MouseUp(int button, double x, double y);
   void MouseClick(int button, double x, double y);
   void MouseDrag(int button, double x, double y);

   Input::InputManager* mInput;

private:
   struct EventToLog {
      enum { DOWN, UP, CLICK, DRAG } type;
      int button;
      double x, y;
   };

   std::queue<EventToLog> mEvents;
};

}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#include <functional>

#include <RGBLogger/Logger.h>
#include "InputEventSystem.h"

namespace CubeWorld
{

namespace Engine
{

void InputEventSystem::Configure(EntityManager&, EventManager&)
{
   assert(mInput != nullptr);

   mInput->OnMouseDown(std::bind(&InputEventSystem::MouseDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
   mInput->OnMouseUp(std::bind(&InputEventSystem::MouseUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
   mInput->OnClick(std::bind(&InputEventSystem::MouseClick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void InputEventSystem::MouseDown(int button, double x, double y)
{
   mEvents.push(EventToLog{EventToLog::DOWN, button, x, y});
}

void InputEventSystem::MouseUp(int button, double x, double y)
{
   mEvents.push(EventToLog{EventToLog::UP, button, x, y});
}

void InputEventSystem::MouseClick(int button, double x, double y)
{
   mEvents.push(EventToLog{EventToLog::CLICK, button, x, y});
}

void InputEventSystem::Update(Engine::EntityManager&, Engine::EventManager& events, TIMEDELTA)
{
   while (!mEvents.empty())
   {
      EventToLog info = mEvents.front();
      mEvents.pop();

      switch (info.type)
      {
      case EventToLog::DOWN:
         events.Emit<MouseDownEvent>(info.button, info.x, info.y);
         break;
      case EventToLog::UP:
         events.Emit<MouseUpEvent>(info.button, info.x, info.y);
         break;
      case EventToLog::CLICK:
         events.Emit<MouseClickEvent>(info.button, info.x, info.y);
         break;
      }
   }
}

}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Paths.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>

#include "../Command/CommandStack.h"
#include "Scrubber.h"

namespace CubeWorld
{

namespace Editor
{

BaseScrubber::BaseScrubber(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options)
   : Image(root, parent, options)
   , mScrubbing(nullptr)
   , mLastPosition(0)
{
}

void BaseScrubber::Receive(const MouseDownEvent& evt)
{
   if (evt.button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return;
   }

   if (ContainsPoint(evt.x, evt.y))
   {
      mLastPosition = glm::tvec2<double>(evt.x, evt.y);
      StartScrubbing();
   }
}

void BaseScrubber::Receive(const MouseUpEvent& evt)
{
   if (evt.button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return;
   }

   if (mScrubbing)
   {
      // Funky time: at this point, the current value represents the NEW state,
      // and mScrubbing represents a command to set it to the OLD state. So we
      // perform the command twice, once immediately to revert to the old state,
      // and then again when it gets placed on the stack to go back to the new
      // state.
      mScrubbing->Do();
      CommandStack::Instance()->Do(std::move(mScrubbing));
   }
}

void BaseScrubber::Update(TIMEDELTA)
{
   if (mScrubbing)
   {
      glm::tvec2<double> mouse = Engine::Window::Instance()->GetInput()->GetRawMousePosition();
      Scrub(mouse.x - mLastPosition.x);
      mLastPosition = mouse;
   }
}

}; // namespace Editor

}; // namespace CubeWorld

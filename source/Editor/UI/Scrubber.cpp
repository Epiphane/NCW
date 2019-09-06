// By Thomas Steinke

#include <cassert>

#include <RGBFileSystem/Paths.h>
#include <Engine/Core/Window.h>
#include <RGBLogger/Logger.h>

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

Engine::UIElement::Action BaseScrubber::MouseDown(const MouseDownEvent& evt)
{
   if (evt.button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return Unhandled;
   }

   if (ContainsPoint(evt.x, evt.y))
   {
      mLastPosition = glm::tvec2<double>(evt.x, evt.y);
      StartScrubbing();
      return Capture;
   }

   return Unhandled;
}

Engine::UIElement::Action BaseScrubber::MouseUp(const MouseUpEvent& evt)
{
   if (evt.button != GLFW_MOUSE_BUTTON_LEFT || !mScrubbing)
   {
      return Unhandled;
   }

   // Funky time: at this point, the current value represents the NEW state,
   // and mScrubbing represents a command to set it to the OLD state. So we
   // perform the command twice, once immediately to revert to the old state,
   // and then again when it gets placed on the stack to go back to the new
   // state.
   mScrubbing->Do();
   CommandStack::Instance().Do(std::move(mScrubbing));

   return Handled;
}

Engine::UIElement::Action BaseScrubber::MouseMove(const MouseMoveEvent& evt)
{
   if (mScrubbing)
   {
      Scrub(evt.x - mLastPosition.x);
      mLastPosition.x = evt.x;
      mLastPosition.y = evt.y;

      return Handled;
   }

   return Unhandled;
}

}; // namespace Editor

}; // namespace CubeWorld

// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/Helpers/Asset.h>

#include "TextButton.h"

namespace CubeWorld
{

namespace UI
{

TextButton::TextButton(
   Engine::UIRoot* root,
   Engine::UIElement* parent,
   const Options& options
)
   : Text(root, parent, options)
   , mClickCallback(options.onClick)
   , mIsHovered(false)
{}

Engine::UIElement::Action TextButton::MouseClick(const MouseClickEvent& evt)
{
   if (mClickCallback && ContainsPoint(evt.x, evt.y))
   {
      mClickCallback();
      return Handled;
   }
   return Unhandled;
}

Engine::UIElement::Action TextButton::MouseMove(const MouseMoveEvent& evt)
{
   if (!mActive || !mClickCallback)
   {
      return Unhandled;
   }

   glm::tvec2<double> mouse = Engine::Window::Instance()->GetInput()->GetRawMousePosition();
   bool hovered = ContainsPoint(mouse.x, mouse.y);
   if (hovered && !mIsHovered)
   {
      RenderText("> " + mText);
      mIsHovered = true;
   }
   else if (!hovered && mIsHovered)
   {
      RenderText(mText);
      mIsHovered = false;
   }

   return mIsHovered ? Handled : Unhandled;
}

}; // namespace UI

}; // namespace CubeWorld

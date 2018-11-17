// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "Button.h"

namespace CubeWorld
{

namespace UI
{

Button::Button(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options)
   : Image(root, parent, options)
   , mCallback(options.onClick)
   , mState(NORMAL)
{
   mHover = mCoords;
   mPress = mCoords;
   mNormal = mCoords;

   if (!options.hoverImage.empty())
   {
      mHover = mTexture->GetImage(options.hoverImage);
   }

   if (!options.pressImage.empty())
   {
      mPress = mTexture->GetImage(options.pressImage);
   }
}

Engine::UIElement::Action Button::MouseDown(const MouseDownEvent& evt)
{
   if (!mActive || evt.button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return Unhandled;
   }

   if (ContainsPoint(evt.x, evt.y))
   {
      SetState(PRESS);
      return Handled;
   }
   return Unhandled;
}

Engine::UIElement::Action Button::MouseUp(const MouseUpEvent& evt)
{
   if (!mActive || evt.button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return Unhandled;
   }

   if (ContainsPoint(evt.x, evt.y))
   {
      if (mCallback && mState == PRESS)
      {
         mCallback();
      }
      SetState(HOVER);
   }
   else
   {
      SetState(NORMAL);
   }
   return Unhandled;
}

Engine::UIElement::Action Button::MouseMove(const MouseMoveEvent& evt)
{
   if (!mActive)
   {
      return Unhandled;
   }

   bool hovered = ContainsPoint(evt.x, evt.y);
   if (mState == NORMAL && hovered)
   {
      SetState(HOVER);
   }
   else if (mState == HOVER && !hovered)
   {
      SetState(NORMAL);
   }

   return hovered ? Handled : Unhandled;
}

void Button::SetState(State state)
{
   if (mState == state)
   {
      return;
   }

   mState = state;
   switch (mState)
   {
   case NORMAL:
      mCoords = mNormal;
      break;
   case HOVER:
      mCoords = mHover;
      break;
   case PRESS:
      mCoords = mPress;
      break;
   }

   Redraw();
}

}; // namespace UI

}; // namespace CubeWorld

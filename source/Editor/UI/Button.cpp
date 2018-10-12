// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>

#include "Button.h"

namespace CubeWorld
{

namespace Editor
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

void Button::Receive(const MouseDownEvent& evt)
{
   if (evt.button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return;
   }

   if (ContainsPoint(evt.x, evt.y))
   {
      SetState(PRESS);
   }
}

void Button::Receive(const MouseUpEvent& evt)
{
   if (evt.button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return;
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
}

void Button::Update(TIMEDELTA dt)
{
   if (mCallback)
   {
      glm::tvec2<double> mouse = Engine::Window::Instance()->GetInput()->GetRawMousePosition();
      bool hovered = ContainsPoint(mouse.x, mouse.y);
      if (mState == NORMAL && hovered)
      {
         SetState(HOVER);
      }
   }

   Image::Update(dt);
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

   // Update the VBO region
   UpdateRegion();
}

}; // namespace Editor

}; // namespace CubeWorld

// By Thomas Steinke

#include <cassert>
#include <algorithm>

#include <Engine/Core/Input.h>
#include <Engine/Core/Paths.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>

#include "ScrollBar.h"

namespace CubeWorld
{

namespace UI
{

ScrollBar::ScrollBar(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options)
   : Image(root, parent, options)
   , mScrubbing(false)
   , mMin(options.min)
   , mRange(options.max - options.min)
   , mCallback(options.onChange)
{}

Engine::UIElement::Action ScrollBar::MouseDown(const MouseDownEvent& evt)
{
   if (mActive && evt.button == GLFW_MOUSE_BUTTON_LEFT)
   {
      mScrubbing = ContainsPoint(evt.x, evt.y);
      return mScrubbing ? Handled : Unhandled;
   }
   return Unhandled;
}

Engine::UIElement::Action ScrollBar::MouseUp(const MouseUpEvent& evt)
{
   if (mActive && evt.button == GLFW_MOUSE_BUTTON_LEFT)
   {
      mScrubbing = false;
      Redraw();
   }
   return Unhandled;
}

void ScrollBar::Update(TIMEDELTA)
{
   if (mScrubbing)
   {
      glm::tvec2<double> mouse = Engine::Window::Instance()->GetInput()->GetRawMousePosition();
      double relative = std::clamp((mouse.x - mFrame.left.int_value()) / mFrame.width.int_value(), 0.0, 1.0);
      SetValue(mMin + mRange * relative);
      if (mCallback)
      {
         mCallback(GetValue());
      }

      Redraw();
   }

   if (Binding::Update())
   {
      Redraw();
   }
}

void ScrollBar::Redraw()
{
   std::vector<Aggregator::ImageData> vertices;
   if (mActive)
   {
      double pixelW = mCoords.z * mTexture->GetWidth();
      double pixelH = mCoords.w * mTexture->GetHeight();
      double height = mFrame.height.value();
      double width = height * pixelW / pixelH;
      double offset = (GetValue() - mMin) / mRange; // [0, 1]

      glm::vec3 bottomLeft = mFrame.GetBottomLeft();
      bottomLeft.x += mFrame.width.value() * offset - pixelW / 2;
      glm::vec3 topRight = bottomLeft + glm::vec3(width, height, 0);

      vertices.push_back({bottomLeft, glm::vec2(mCoords.x, mCoords.y + mCoords.w)});
      vertices.push_back({topRight, glm::vec2(mCoords.x + mCoords.z, mCoords.y)});
   }
   else
   {
      vertices.push_back({glm::vec3(0),glm::vec2(0)});
      vertices.push_back({glm::vec3(0),glm::vec2(0)});
   }

   mRegion.Set(vertices.data());
}

}; // namespace UI

}; // namespace CubeWorld

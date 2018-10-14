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
   : UIElement(root, parent)
   , mScrubbing(false)
   , mMin(options.min)
   , mRange(options.max - options.min)
   , mCallback(options.onChange)
   , mRegion(root->Reserve<Aggregator::Image>(2))
{
   LOG_DEBUG("Loading %1", Paths::Canonicalize(options.filename));
   Maybe<Engine::Graphics::Texture*> maybeTexture = Engine::Graphics::TextureManager::Instance()->GetTexture(options.filename);
   if (!maybeTexture)
   {
      LOG_ERROR(maybeTexture.Failure().WithContext("Failed loading %1", options.filename).GetMessage());
      return;
   }
   mTexture = *maybeTexture;

   mCoords = glm::vec4(0, 0, 1, 1);
   if (!options.image.empty())
   {
      mCoords = mTexture->GetImage(options.image);
   }

   root->GetAggregator<Aggregator::Image>()->ConnectToTexture(mRegion, mTexture->GetTexture());
}

void ScrollBar::Receive(const MouseDownEvent& evt)
{
   if (evt.button == GLFW_MOUSE_BUTTON_LEFT)
   {
      mScrubbing = ContainsPoint(evt.x, evt.y);
   }
}

void ScrollBar::Receive(const MouseUpEvent& evt)
{
   if (evt.button == GLFW_MOUSE_BUTTON_LEFT)
   {
      mScrubbing = false;
      Redraw();
   }
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
   double offset = (GetValue() - mMin) / mRange;
   std::vector<Aggregator::ImageData> vertices{
      { mFrame.GetBottomLeft(), glm::vec2(mCoords.x, mCoords.y) },
      { mFrame.GetTopRight(), glm::vec2(mCoords.x + mCoords.z, mCoords.y + mCoords.w) },
   };

   mRegion.Set(vertices.data());
}

}; // namespace UI

}; // namespace CubeWorld

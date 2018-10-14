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
{
   root->Subscribe<MouseClickEvent>(*this);
}

void TextButton::Receive(const MouseClickEvent& evt)
{
   if (mClickCallback && ContainsPoint(evt.x, evt.y))
   {
      mClickCallback();
   }
}

void TextButton::Update(TIMEDELTA dt)
{
   if (mClickCallback)
   {
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
   }

   Text::Update(dt);
}

}; // namespace UI

}; // namespace CubeWorld

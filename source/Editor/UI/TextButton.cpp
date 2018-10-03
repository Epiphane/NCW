// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Shared/Helpers/Asset.h>

#include "TextButton.h"

namespace CubeWorld
{

namespace Editor
{

TextButton::TextButton(
   Bounded& parent,
   const Options& options
)
   : Text(parent, options)
   , mClickCallback(options.onClick)
   , mIsHovered(false)
{
}

void TextButton::MouseClick(int button, double x, double y)
{
   if (mClickCallback && ContainsPoint(x, y))
   {
      mClickCallback();
   }
}

void TextButton::Update(TIMEDELTA dt)
{
   if (mClickCallback)
   {
      glm::tvec2<double> mouse = AbsoluteToRelative(Engine::Window::Instance()->GetInput()->GetRawMousePosition());
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

}; // namespace Editor

}; // namespace CubeWorld

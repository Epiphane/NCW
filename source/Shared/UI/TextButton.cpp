// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Window.h>
#include <RGBLogger/Logger.h>
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
   const Options& options,
   const std::string& name
)
   : Text(root, parent, options, name)
   , mClickCallback(options.onClick)
   , mIsHovered(false)
   , mIsFocused(false)
{}

void TextButton::Focus()
{
   if (mIsFocused)
   {
      return;
   }

   RenderText("> " + mText);
   RecalculateSize();
   mIsFocused = true;
}

void TextButton::Unfocus()
{
   if (!mIsFocused)
   {
      return;
   }

   if (!mIsHovered)
   {
      RenderText(mText);
      RecalculateSize();
   }
   mIsFocused = false;
}

Engine::UIElement::Action TextButton::MouseClick(const MouseClickEvent& evt)
{
   if (mClickCallback && ContainsPoint(evt.x, evt.y))
   {
      mClickCallback();
      return Handled;
   }
   return Unhandled;
}

void TextButton::Update(TIMEDELTA)
{
   if (!mActive || !mClickCallback)
   {
      return;
   }

   glm::tvec2<double> mouse = mpRoot->GetInput()->GetRawMousePosition();
   bool hovered = ContainsPoint(mouse.x, mouse.y);
   if (hovered && !mIsHovered)
   {
      RenderText("> " + mText);
      RecalculateSize(); // TODO-EF: This is changing constraints when the constraint solver has
                         //            already run this frame. Needs looking into.
      mIsHovered = true;
   }
   else if (!mIsFocused && !hovered && mIsHovered)
   {
      RenderText(mText);
      RecalculateSize();
      mIsHovered = false;
   }
}

}; // namespace UI

}; // namespace CubeWorld

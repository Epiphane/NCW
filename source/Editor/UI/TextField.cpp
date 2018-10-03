// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Window.h>

#include "TextField.h"

namespace CubeWorld
{

namespace Editor
{

TextField::TextField(
   Bounded& parent,
   const Options& options
)
   : Text(parent, options)
   , mChangeCallback(options.onChange)
   , mIsFocused(false)
   , mKeyCallbacks{}
{
   auto onAlpha = std::bind(&TextField::OnAlphaKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
   for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; key++)
   {
      mKeyCallbacks.push_back(Engine::Window::Instance()->GetInput()->AddCallback(Engine::Input::Key(key), onAlpha));
      mKeyCallbacks.push_back(Engine::Window::Instance()->GetInput()->AddCallback(Engine::Input::ShiftKey(key), onAlpha));
   }
   mKeyCallbacks.push_back(Engine::Window::Instance()->GetInput()->AddCallback(Engine::Input::Key(GLFW_KEY_BACKSPACE), onAlpha));
   mKeyCallbacks.push_back(Engine::Window::Instance()->GetInput()->AddCallback(Engine::Input::Key(GLFW_KEY_ENTER), onAlpha));
}

void TextField::OnAlphaKey(int key, int action, int mods)
{
   if (!mIsFocused || action != GLFW_PRESS)
   {
      return;
   }

   switch (key)
   {
   case GLFW_KEY_BACKSPACE:
      if (mText.size() > 1)
      {
         // Erase the second to last character
         mText.erase(mText.end() - 2, mText.end() - 1);
         RenderText(mText);
      }
      break;
   case GLFW_KEY_ENTER:
      mIsFocused = false;
      mText.pop_back();
      mChangeCallback(mText);
      RenderText(mText);
      break;
   default:
      // It's a letter
      char ch = static_cast<char>((key - GLFW_KEY_A) + 'a');
      if ((mods & GLFW_MOD_SHIFT) != 0)
      {
         ch += 'A' - 'a';
      }
      mText.insert(mText.end() - 1, ch);
      RenderText(mText);
   }
}

void TextField::MouseClick(int button, double x, double y)
{
   if (button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return;
   }

   bool wasFocused = mIsFocused;
   mIsFocused = ContainsPoint(x, y);
   if (mIsFocused && !wasFocused)
   {
      mText.push_back('_');
      RenderText(mText);
   }
   else if (!mIsFocused && wasFocused)
   {
      mText.pop_back();
      RenderText(mText);
      if (mChangeCallback)
      {
         mChangeCallback(mText);
      }
   }
}

}; // namespace Editor

}; // namespace CubeWorld

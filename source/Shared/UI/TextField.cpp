// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Window.h>
#include <Engine/UI/UIRootDep.h>

#include "TextField.h"

namespace CubeWorld
{

namespace UI
{

TextField::TextField(Engine::UIRootDep* root, Engine::UIElementDep* parent, const Options& options)
   : Text(root, parent, options)
   , mChangeCallback(options.onChange)
   , mIsFocused(false)
   , mKeyCallbacks{}
{
   auto onAlpha = std::bind(&TextField::OnAlphaKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
   for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; key++)
   {
      mKeyCallbacks.push_back(root->GetInput()->AddCallback(Engine::Window::Key(key), onAlpha));
      mKeyCallbacks.push_back(root->GetInput()->AddCallback(Engine::Window::ShiftKey(key), onAlpha));
   }
   for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; key++)
   {
      mKeyCallbacks.push_back(root->GetInput()->AddCallback(Engine::Window::Key(key), onAlpha));
      mKeyCallbacks.push_back(root->GetInput()->AddCallback(Engine::Window::ShiftKey(key), onAlpha));
   }
   mKeyCallbacks.push_back(root->GetInput()->AddCallback(Engine::Window::Key(GLFW_KEY_SPACE), onAlpha));
   mKeyCallbacks.push_back(root->GetInput()->AddCallback(Engine::Window::Key(GLFW_KEY_BACKSPACE), onAlpha));
   mKeyCallbacks.push_back(root->GetInput()->AddCallback(Engine::Window::Key(GLFW_KEY_ENTER), onAlpha));
}

void TextField::AddCharacter(char ch)
{
   mText.insert(mText.end() - 1, ch);
   RenderText(mText);
}

void TextField::OnAlphaKey(int key, int action, int mods)
{
   if (!mActive || !mIsFocused || action != GLFW_PRESS)
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
   case GLFW_KEY_SPACE:
      AddCharacter(' ');
      break;
   case GLFW_KEY_0:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? ')' : '0');
      break;
   case GLFW_KEY_1:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '!' : '1');
      break;
   case GLFW_KEY_2:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '@' : '2');
      break;
   case GLFW_KEY_3:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '#' : '3');
      break;
   case GLFW_KEY_4:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '$' : '4');
      break;
   case GLFW_KEY_5:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '%' : '5');
      break;
   case GLFW_KEY_6:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '^' : '6');
      break;
   case GLFW_KEY_7:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '&' : '7');
      break;
   case GLFW_KEY_8:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '*' : '8');
      break;
   case GLFW_KEY_9:
      AddCharacter((mods & GLFW_MOD_SHIFT) != 0 ? '(' : '9');
      break;
   default:
      // It's a letter
      char ch = static_cast<char>((key - GLFW_KEY_A) + 'a');
      if ((mods & GLFW_MOD_SHIFT) != 0)
      {
         ch += 'A' - 'a';
      }
      AddCharacter(ch);
      break;
   }
}

Engine::UIElementDep::Action TextField::MouseClick(const MouseClickEvent& evt)
{
   if (!mActive || evt.button != GLFW_MOUSE_BUTTON_LEFT)
   {
      return Unhandled;
   }

   bool wasFocused = mIsFocused;
   mIsFocused = ContainsPoint(evt.x, evt.y);
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
   return mIsFocused ? Handled : Unhandled;
}

}; // namespace UI

}; // namespace CubeWorld

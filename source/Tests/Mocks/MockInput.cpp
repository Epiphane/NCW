// By Thomas Steinke

#include <codecvt>
#include <locale>

#include "MockInput.h"

namespace CubeWorld
{

namespace Test
{

//
//
//
MockInput::MockInput()
{
   Reset();
}

MockInput::~MockInput() = default;

//
//
//
void MockInput::Reset()
{
   Input::Reset();
   mMouseLocked = false;
   mMousePosition = {0, 0};
   mMouseMovement = {0, 0};
   mLastMouseScroll = {0, 0};
   memset(mKeyPressed, 0, sizeof(mKeyPressed));
   memset(mMousePressed, 0, sizeof(mMousePressed));
   memset(mMouseDragging, 0, sizeof(mMouseDragging));
   memset(mMousePressOrigin, 0, sizeof(mMousePressOrigin));
}

//
//
//
void MockInput::Update()
{
   mLastMouseScroll = {0, 0};
}

//
//
//
void MockInput::SetMousePosition(glm::tvec2<double> pos)
{
   if (mMouseLocked)
   {
      return;
   }

   mMousePosition = pos;
}

//
//
//
void MockInput::MoveMouse(glm::tvec2<double> amount)
{
   if (mMouseLocked)
   {
      return;
   }

   mMousePosition += amount;
   for (int button = GLFW_MOUSE_BUTTON_1; button < GLFW_MOUSE_BUTTON_LAST; ++button)
   {
      if (mMousePressed[button])
      {
         mMouseDragging[button] = true;
      }
   }
}

//
//
//
void MockInput::MouseDown(int button)
{
   mMousePressed[button] = true;
   if (mMouseDownCallback)
   {
      mMouseDownCallback(button, mMousePosition.x, mMousePosition.y);
   }
}

//
//
//
void MockInput::MouseUp(int button)
{
   mMousePressed[button] = false;
   mMouseDragging[button] = false;
   if (mMouseUpCallback)
   {
      mMouseUpCallback(button, mMousePosition.x, mMousePosition.y);
   }
}

//
//
//
void MockInput::Click(int button, glm::tvec2<double> pos)
{
   SetMousePosition(pos);
   if (mMouseClickCallback)
   {
      mMouseClickCallback(button, mMousePosition.x, mMousePosition.y);
   }
}

//
//
//
void MockInput::SetScroll(glm::tvec2<double> amount)
{
   mLastMouseScroll = amount;
}

//
//
//
void MockInput::KeyDown(int key, int mods)
{
   mKeyPressed[key] = true;
   TriggerKeyCallbacks(key, GLFW_PRESS, mods);
}

//
//
//
void MockInput::KeyUp(int key, int mods)
{
   mKeyPressed[key] = false;
   TriggerKeyCallbacks(key, GLFW_RELEASE, mods);
}

//
//
//
void MockInput::Press(int key, int mods)
{
   KeyDown(key, mods);
   KeyUp(key, mods);
}

//
//
//
void MockInput::Type(unsigned int codePoint)
{
   TriggerCharCallbacks(codePoint);
}

//
//
//
void MockInput::Type(const std::string& str)
{
   std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
   std::u32string string = conv.from_bytes(str);
   for (const char32_t& c : string)
   {
      Type(c);
   }
}

}; // namespace Test

}; // namespace CubeWorld

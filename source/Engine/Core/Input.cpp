// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>
#include <cmath>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>

#include "Input.h"

namespace CubeWorld
{

namespace Engine
{

void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods)
{
   Input *input = (Input *)glfwGetWindowUserPointer(window);
   // TODO validate this is a real Input

   if (action == GLFW_PRESS && input->mKeyCallbacks[key] != nullptr)
   {
      // Circle the ring, invoking callbacks.
      Input::KeyCallbackLink* start = input->mKeyCallbacks[key];
      Input::KeyCallbackLink* link = start;
      do
      {
         // Check for equivalence on the modifier keys.
         // Future consideration, is a superset okay?
         if (link->mods == mods && link->callback)
         {
            link->callback(key, action, mods);
         }
         link = link->next;
      } while (link != start);
   }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
   Input *input = (Input *)glfwGetWindowUserPointer(window);
   // TODO validate this is a real Input

   input->mMouseScroll[0] += xoffset;
   input->mMouseScroll[1] += yoffset;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/)
{
   Input *input = (Input *)glfwGetWindowUserPointer(window);
   // TODO validate this is a real Input

   double w_width = static_cast<double>(input->mWindow->Width());
   double w_height = static_cast<double>(input->mWindow->Height());

   double xpos, ypos;
   glfwGetCursorPos(window, &xpos, &ypos);
   input->mMousePressed[button] = (action == GLFW_PRESS);

   // If the mouse is pressed, then try for a MouseDown callback.
   if (input->mMousePressed[button])
   {
      if (input->mMouseDownCallback)
      {
         input->mMouseDownCallback(button, xpos / w_width, ypos / w_height);
      }
      input->mMousePressOrigin[button] = {xpos, ypos};
   }
   // Otherwise, it's released - check whether we were dragging (MouseUp)
   // or not (MouseClick).
   else
   {
      if (input->mMouseDragging[button])
      {
         if (input->mMouseUpCallback)
         {
            input->mMouseUpCallback(button, xpos / w_width, ypos / w_height);
         }
         input->mMouseDragging[button] = false;
      }
      else
      {
         if (input->mMouseClickCallback)
         {
            input->mMouseClickCallback(button, xpos / w_width, ypos / w_height);
         }
      }
   }
}

Input::Input()
   : mWindow{nullptr}
   , mKeyCallbacks{nullptr}
   , mMouseLocked{false}
   , mMouseDownCallback{nullptr}
   , mMouseUpCallback{nullptr}
   , mMouseClickCallback{nullptr}
   , mMouseDragCallback{nullptr}
   , mMousePosition{0, 0}
   , mMouseMovement{0, 0}
   , mLastMouseScroll{0, 0}
   , mMouseScroll{0, 0}
   , mMousePressed{false}
   , mMouseDragging{false}
   , mMousePressOrigin{{0, 0}}
{
}

Input::~Input()
{
}

void Input::Initialize(Window* window)
{
   assert(window != nullptr);
   mWindow = window;
   Clear();

   glfwMakeContextCurrent(window->mGLFW);
   glfwSetWindowUserPointer(window->mGLFW, this);

   // Ensure we can capture the escape key being pressed
   glfwSetInputMode(window->mGLFW, GLFW_STICKY_KEYS, GL_TRUE);

   glfwSetKeyCallback(window->mGLFW, &keyCallback);
   glfwSetScrollCallback(window->mGLFW, &scrollCallback);
   glfwSetMouseButtonCallback(window->mGLFW, &mouseButtonCallback);
}

void Input::SetMouseLock(bool locked)
{
   mMouseLocked = locked;

   if (locked) {
      glfwSetInputMode(mWindow->mGLFW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   }
   else {
      glfwSetInputMode(mWindow->mGLFW, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }
}

void Input::Clear()
{
   memset(mKeyCallbacks, 0, sizeof(mKeyCallbacks));
   mMouseLocked = false;
   mMouseDownCallback = nullptr;
   mMouseUpCallback = nullptr;
   mMouseClickCallback = nullptr;
   mMouseDragCallback = nullptr;
   mMousePosition = {0, 0};
   mMouseMovement = {0, 0};
   mLastMouseScroll = {0, 0};
   mMouseScroll = {0, 0};
   memset(mMousePressed, 0, sizeof(mMousePressed));
   memset(mMouseDragging, 0, sizeof(mMouseDragging));
   memset(mMousePressOrigin, 0, sizeof(mMousePressOrigin));
}

void Input::Update()
{
   // Don't lock up mouse
   glfwGetCursorPos(mWindow->mGLFW, &mMousePosition.x, &mMousePosition.y);

   mLastMouseScroll = mMouseScroll;
   mMouseScroll = {0, 0};

   if (!mMouseLocked) {
      for (int button = GLFW_MOUSE_BUTTON_1; button < GLFW_MOUSE_BUTTON_LAST; ++button)
      {
         if (mMousePressed[button] && !mMouseDragging[button])
         {
            double dist = (mMousePosition - mMousePressOrigin[button]).length();
            if (dist > 2)
            {
               mMouseDragging[button] = true;
            }
         }

         if (mMouseDragCallback && mMouseDragging[button])
         {
            mMouseDragCallback(button, mMousePosition.x / mWindow->Width(), mMousePosition.y / mWindow->Height());
         }
      }
   }
   else
   {
      glm::tvec2<double> middle = glm::tvec2<double>(mWindow->Width(), mWindow->Height()) / 2.0;

      mMouseMovement = middle - mMousePosition;
      // Edge case: window initialization makes things funky
      if (std::abs(mMouseMovement.x) > 200 ||
          std::abs(mMouseMovement.y) > 200 ||
          mMousePosition == glm::tvec2<double>(0)) {
         mMouseMovement = {0, 0};
      }

      glfwSetCursorPos(mWindow->mGLFW, middle.x, middle.y);
      mMousePosition = middle;
   }
}

bool Input::IsKeyDown(int key) const
{
   return glfwGetKey(mWindow->mGLFW, key) == GLFW_PRESS;
}

void Input::RemoveCallback(std::unique_ptr<KeyCallbackLink> link)
{
   // link will be deconstructed at the end of this function, which is now the owner.
}

void Input::RemoveCallback(KeyCallbackLink* link)
{
   link->next->prev = link->prev;
   link->prev->next = link->next;

   // See if we're emptying a ring.
   if (mKeyCallbacks[link->key] == link)
   {
      if (link->next == link)
      {
         mKeyCallbacks[link->key] = nullptr;
      }
      else
      {
         mKeyCallbacks[link->key] = link->next;
      }
   }

   // Not really necessary, but it helps tie up the "complete disconnect" idea.
   link->next = link->prev = nullptr;
}

std::unique_ptr<Input::KeyCallbackLink> Input::AddCallback(int key, input_key_callback cb)
{
   return AddCallback(Key(key), cb);
}

std::unique_ptr<Input::KeyCallbackLink> Input::AddCallback(KeyCombination key, input_key_callback cb)
{
   std::unique_ptr<KeyCallbackLink> link = std::make_unique<KeyCallbackLink>(this, key, cb);

   if (mKeyCallbacks[key.key] == nullptr)
   {
      mKeyCallbacks[key.key] = link.get();
      link->next = mKeyCallbacks[key.key];
      link->prev = mKeyCallbacks[key.key];
   }
   else
   {
      link->next = mKeyCallbacks[key.key];
      link->prev = link->next->prev;
      link->prev->next = link.get();
      link->next->prev = link.get();
   }

   return std::move(link);
}

std::vector<std::unique_ptr<Input::KeyCallbackLink>> Input::AddCallback(const std::vector<KeyCombination>& keys, input_key_callback cb)
{
   std::vector<std::unique_ptr<Input::KeyCallbackLink>> result;

   for (auto key : keys)
   {
      result.push_back(std::move(AddCallback(key, cb)));
   }

   return std::move(result);
}

}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Maybe.h"
#include "Singleton.h"
#include "../Graphics/VAO.h"

namespace CubeWorld
{

namespace Engine
{

// -------------------------- IMPORTANT NOTE ---------------------------
//              (see VBO.h for a more thorough explanation)
//
// Right now, multi-window isn't supported, so to make that explicit
// the Window class is a Singleton. One day, it would be nice to enable
// multi-window support, but that would require lots of change and it's
// not really worth pretending it's possible right now.

// Wrapper for a glfwWindow, allowing for management of input, etc
class Window : public Singleton<Window>
{
public:
   struct Options
   {
      //
      // Window title;
      //
      std::string title;

      //
      // Fullscreen?
      //
      bool fullscreen = true;

      //
      // Width and height of the window. -1 means default to primary monitor size.
      //
      int32_t width = -1;
      int32_t height = -1;

      //
      // Starting position of the window, relative to the center of the screen.
      //
      int32_t x = 0;
      int32_t y = 0;

      //
      // Window clear color
      //
      float r = 0.0f;
      float g = 0.0f;
      float b = 0.0f;
      float a = 1.0f;

      //
      // Whether to lock the cursor inside the window
      //
      bool lockCursor = true;
   };

protected:
   Window();
   ~Window();
   friend class Singleton<Window>;

public:
   Maybe<void> Initialize(const Options& options);

   bool IsReady() { return window != nullptr; }

public:
   inline void Clear() { Use(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
   inline void SwapBuffers() { glfwSwapBuffers(window); }
   inline bool ShouldClose() { return glfwWindowShouldClose(window) != 0; }
   inline void Focus() { glfwFocusWindow(window); }
   void LockCursor();
   void UnlockCursor();

   int32_t Width() { return mOptions.width; }
   int32_t Height() { return mOptions.height; }
   GLFWwindow* get() { return window; }

   void Use();

private:
   GLFWwindow* window;
   Graphics::VAO mVAO;

   Options mOptions;
};

}; // namespace Engine

}; // namespace CubeWorld

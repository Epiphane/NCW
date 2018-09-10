// By Thomas Steinke

#pragma once

#include <GL/includes.h>

#include "Bounded.h"
#include "Input.h"
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
class Window : public Bounded, public Singleton<Window>
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
   // Instantiate this by calling Window::Instance()->Initialize(...);
   Window();
   ~Window();
   friend class Singleton<Window>;

public:
   // Returns a pointer to the instance that already exists, for chaining.
   Maybe<Window*> Initialize(const Options& options);

   bool IsReady() { return mGLFW != nullptr; }

public:
   // Reconfigure the window at runtime
   inline void SetTitle(const std::string& title) { glfwSetWindowTitle(mGLFW, title.c_str()); }

public:
   // Implement Bounded
   uint32_t Width() const override { return mOptions.width; }
   uint32_t Height() const override { return mOptions.height; }

public:
   // Typical GLFW methods
   void Clear();
   void SwapBuffers();
   void SetShouldClose(bool close);
   bool ShouldClose();
   void Focus();

public:
   Input* GetInput() { return &mInput; }

private:
   // They might as well be one class, but it would be a very large unwieldy class.
   friend class Input;
   Input mInput;

   GLFWwindow* mGLFW;
   Graphics::VAO mVAO;

   Options mOptions;
};

}; // namespace Engine

}; // namespace CubeWorld

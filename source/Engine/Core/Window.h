// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Engine/Graphics/VAO.h>

namespace CubeWorld
{

namespace Engine
{

// Wrapper for a glfwWindow, allowing for management of input, etc
class Window
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

public:
   Window(Options options);
   ~Window();

   bool IsReady() { return window != nullptr; }

public:
   inline void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
   inline void SwapBuffers() { glfwSwapBuffers(window); }
   inline bool ShouldClose() { return glfwWindowShouldClose(window) != 0; }
   void LockCursor();
   void UnlockCursor();

   int32_t Width() { return mOptions.width; }
   int32_t Height() { return mOptions.height; }
   GLFWwindow* get() { return window; }

private:
   GLFWwindow * window;
   Graphics::VAO mVAO;

   Options mOptions;

public:

};

}; // namespace Engine

}; // namespace CubeWorld

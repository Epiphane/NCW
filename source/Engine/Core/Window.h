// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../Graphics/VAO.h"

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

public:
   Window(Options options);
   ~Window();

   bool IsReady() { return window != nullptr; }

public:
   inline void Clear() { Use(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
   inline void SwapBuffers() { glfwSwapBuffers(window); }
   inline bool ShouldClose() { return glfwWindowShouldClose(window) != 0; }
   inline void Focus() {  }
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

private:
   // Each window contains a reference to its previous and next window, so that
   // if one is removed we still have an "origin" window.
   Window* prev;
   Window* next;

   static Window* root;
};

}; // namespace Engine

}; // namespace CubeWorld

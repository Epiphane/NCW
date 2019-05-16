// By Thomas Steinke

#pragma once

#include <GL/includes.h>

#include "Bounded.h"
#include "Input.h"
#include <RGBDesignPatterns/Maybe.h>
#include <RGBDesignPatterns/Singleton.h>
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
class Window : public Input, public Bounded, public Singleton<Window>
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
   // Instantiate this by calling Window::Instance().Initialize(...);
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
   uint32_t GetX() const override { return 0; }
   uint32_t GetY() const override { return 0; }
   uint32_t GetWidth() const override { return (uint32_t)mOptions.width; }
   uint32_t GetHeight() const override { return (uint32_t)mOptions.height; }

public:
   // Typical GLFW methods
   void Clear();
   void SwapBuffers();
   void SetShouldClose(bool close);
   bool ShouldClose();
   void Focus();

   GLFWwindow* get() { return mGLFW; }

private:
   GLFWwindow* mGLFW;
   Graphics::VAO mVAO;

   Options mOptions;

private:
   //
   // Used for listening to inputs
   //
   friend void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
   friend void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
   friend void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

public:
   // Helpers for key combinations
   static KeyCombination Key(int key) { return KeyCombination{key, 0}; }
   static KeyCombination ShiftKey(int key) { return KeyCombination{key, GLFW_MOD_SHIFT}; }
   static KeyCombination CtrlKey(int key) { return KeyCombination{key, GLFW_MOD_CONTROL}; }
   static KeyCombination AltKey(int key) { return KeyCombination{key, GLFW_MOD_ALT}; }
   static KeyCombination SuperKey(int key) { return KeyCombination{key, GLFW_MOD_SUPER}; }
   static KeyCombination CtrlShiftKey(int key) { return KeyCombination{key, GLFW_MOD_SHIFT | GLFW_MOD_CONTROL}; }

public:
   //
   // Overrides from Input base class.
   //
   void Reset() override;
   void Update() override;
   bool IsKeyDown(int key) const override;
   bool IsMouseDown(int button) const override { return mMousePressed[button]; }
   bool IsDragging(int button) const override { return mMouseDragging[button]; }
   glm::tvec2<double> GetRawMousePosition() const override;
   glm::tvec2<double> GetMousePosition() const override;
   glm::tvec2<double> GetMouseMovement() const override { return mMouseMovement; }
   glm::tvec2<double> GetMouseScroll() const override { return mLastMouseScroll; }
   void SetMouseLock(bool locked) override;
   bool IsMouseLocked() const override { return mMouseLocked; }

private:
   bool mMouseLocked = false;

   glm::tvec2<double> mMousePosition;
   glm::tvec2<double> mMouseMovement;

   glm::tvec2<double> mLastMouseScroll;
   glm::tvec2<double> mMouseScroll; // Accumulated between updates.

   bool mMousePressed[GLFW_MOUSE_BUTTON_LAST];
   bool mMouseDragging[GLFW_MOUSE_BUTTON_LAST];
   glm::tvec2<double> mMousePressOrigin[GLFW_MOUSE_BUTTON_LAST];
};

}; // namespace Engine

}; // namespace CubeWorld

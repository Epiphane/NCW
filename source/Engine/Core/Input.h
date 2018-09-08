// By Thomas Steinke

#pragma once

#include <functional>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace CubeWorld
{

namespace Engine
{

class Window;

namespace Input
{

using input_key_callback = std::function<void(void)>;
using input_alpha_callback = std::function<void(char)>;
using mouse_button_callback = std::function<void(int, double, double)>;

class InputManager
{
public:
   // Initializes the singleton with a window. This must be called first.
   static InputManager* Initialize(Window* window);
   static InputManager* Instance();

   InputManager(Window* window);
   ~InputManager();

private:
   static uint32_t nInstances;
   static std::unique_ptr<InputManager> sInstance;

   Window* window;
   inline const void SetContextCurrent() const;

   input_key_callback keyCallbacks[GLFW_KEY_LAST] = {0};
   input_alpha_callback alphaCallback = nullptr;

   friend void keyCallback(GLFWwindow* window, int key, int scancode, int action, int modes);
   friend void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
   friend void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

public:
   //
   // Set whether the mouse should be locked to the center of the screen.
   //
   void SetMouseLock(bool locked);
   bool IsMouseLocked() { return mouseLocked; }

private:
   bool mouseLocked = false;

public:
   void Clear();
   void Update();
   bool IsKeyDown(int key) const;

   //
   // Get the mouse's current position, and most recent movement.
   //
   // position and movement must refer to an array of size 2+, or nullptr
   //
   void GetMouse(double *position, double *movement) const;

   // Position must refer to an array of size 2+
   void GetMousePos(double *position) const;

   // Movement must refer to an array of size 2+
   void GetMouseMovement(double *movement) const;
   
   // Scroll must refer to an array of size 2+
   void GetMouseScroll(double *scroll) const;

   //
   // Register a callback to be called on any alphabetical key
   //
   void OnAlphaKey(input_alpha_callback cb);

   //
   // Register a callback to a specific key press
   //
   void SetCallback(int key, input_key_callback cb);

   //
   // Register a callback to mouse events
   //
   void OnMouseDown(mouse_button_callback cb);
   void OnMouseUp(mouse_button_callback cb);
   void OnClick(mouse_button_callback cb);
   void OnDrag(mouse_button_callback cb);

private:
   mouse_button_callback mouseDownCallback = nullptr;
   mouse_button_callback mouseUpCallback = nullptr;
   mouse_button_callback mouseClickCallback = nullptr;
   mouse_button_callback mouseDragCallback = nullptr;

   double mousePosition[2];
   double mouseMovement[2];

   double mouseScroll[2];
   double _mouseScroll[2]; // Accumulated between updates.

   bool mousePressed[GLFW_MOUSE_BUTTON_LAST];
   bool mouseDragging[GLFW_MOUSE_BUTTON_LAST];
   double mousePressOrigin[GLFW_MOUSE_BUTTON_LAST * 2];
};

}; // namespace Input

}; // namespace Engine

}; // namespace CubeWorld

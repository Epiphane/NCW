// By Thomas Steinke

#pragma once

#include <memory>
#include <GLFW/glfw3.h>

namespace CubeWorld
{

namespace Engine
{

class Window;

namespace Input
{

typedef void(*input_key_callback)(void);
typedef void(*input_alpha_callback)(char);

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

public:
   //
   // Set whether the mouse should be locked to the center of the screen.
   //
   void SetMouseLock(bool locked);

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
   //
   // Register a callback to be called on any alphabetical key
   //
   void OnAlphaKey(input_alpha_callback cb);
   //
   // Register a callback to a specific key press
   //
   void SetCallback(int key, input_key_callback cb);

private:
   double mousePosition[2];
   double mouseMovement[2];
};

}; // namespace Input

}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>

#include "Input.h"

namespace CubeWorld
{

namespace Engine
{

namespace Input
{

uint32_t InputManager::nInstances = 0;

void keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*modes*/) {
   InputManager *input = InputManager::Instance();

   if (action == GLFW_PRESS && input->keyCallbacks[key])
      input->keyCallbacks[key]();

   if (input->alphaCallback && action == GLFW_PRESS && key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
      input->alphaCallback(static_cast<char>(key) - GLFW_KEY_A + 'A');
   }
}
std::unique_ptr<InputManager> InputManager::sInstance = nullptr;

inline const void InputManager::SetContextCurrent() const
{
   //if (nInstances > 1)
   //{
   glfwMakeContextCurrent(window->get());
   //}
}

InputManager::InputManager(Window* window) : window(window), mouseLocked(false), keyCallbacks{0}, mousePosition{0}, mouseMovement{0}
{
   assert(window != nullptr);
   glfwMakeContextCurrent(window->get());
   glfwSetWindowUserPointer(window->get(), this);

   // Ensure we can capture the escape key being pressed
   glfwSetInputMode(window->get(), GLFW_STICKY_KEYS, GL_TRUE);

   glfwSetKeyCallback(window->get(), &keyCallback);

   ++nInstances;
}

InputManager::~InputManager()
{
   --nInstances;
}

InputManager* InputManager::Initialize(Window* window)
{
   sInstance = std::make_unique<InputManager>(window);
   return sInstance.get();
}

InputManager* InputManager::Instance()
{
   assert(sInstance != nullptr);
   return sInstance.get();
}

void InputManager::SetMouseLock(bool locked)
{
   SetContextCurrent();
   mouseLocked = locked;

   if (locked) {
      glfwSetInputMode(window->get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   }
   else {
      glfwSetInputMode(window->get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }
}

void InputManager::Clear()
{
   memset(mousePosition, 0, sizeof(mousePosition));
   memset(mouseMovement, 0, sizeof(mouseMovement));
   memset(keyCallbacks, 0, sizeof(keyCallbacks));
}

void InputManager::Update()
{
   // Don't lock up mouse
   SetContextCurrent();

   glfwGetCursorPos(window->get(), &mousePosition[0], &mousePosition[1]);

   if (!mouseLocked) {
      return;
   }

   // Update camera
   double w_2 = static_cast<double>(window->Width()) / 2;
   double h_2 = static_cast<double>(window->Height()) / 2;

   mouseMovement[0] = w_2 - mousePosition[0];
   mouseMovement[1] = h_2 - mousePosition[1];
   // Edge case: window initialization
   if (std::abs(mouseMovement[0]) > 200 || std::abs(mouseMovement[1]) > 200 || (mousePosition[0] == 0 && mousePosition[1] == 0)) {
      mouseMovement[0] = mouseMovement[1] = 0;
   }

   if (mouseLocked)
   {
      glfwSetCursorPos(window->get(), w_2, h_2);
      mousePosition[0] = w_2;
      mousePosition[1] = h_2;
   }
}

bool InputManager::IsKeyDown(int key) const
{
   SetContextCurrent();
   return glfwGetKey(window->get(), key) == GLFW_PRESS;
}

void InputManager::GetMouse(double *position, double *movement) const
{
   if (movement != nullptr)
   {
      memcpy(movement, mouseMovement, sizeof(mouseMovement));
   }

   if (position != nullptr)
   {
      memcpy(position, mousePosition, sizeof(mousePosition));
   }
}

void InputManager::OnAlphaKey(input_alpha_callback cb)
{
   alphaCallback = cb;
}

void InputManager::SetCallback(int key, input_key_callback cb)
{
   if (keyCallbacks[key])
   {
      LOG_WARNING("Overwriting key callback for %c", key);
   }

   keyCallbacks[key] = cb;
}

}; // namespace Input

}; // namespace Engine

}; // namespace CubeWorld
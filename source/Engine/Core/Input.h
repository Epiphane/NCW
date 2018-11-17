// By Thomas Steinke

#pragma once

#include <bitset>
#include <functional>
#include <memory>
#include <glm/glm.hpp>
#include <GL/includes.h>

namespace CubeWorld
{

namespace Engine
{

// Forward declaration so the Input class can refer to it.
class Window;

//
// Callbacks for tying into input events:
//

//
// Called when a specific key is pressed.
// Arguments are (in order):
//  1. GLFW key code (e.g. GLFW_KEY_SPACE)
//  2. Action (e.g. GLFW_PRESS)
//  3. Mods (e.g. GLFW_MOD_SHIFT | GLFW_MOD_CONTROL)
//
using input_key_callback = std::function<void(int, int, int)>;

//
// Called for any event involving a mouse button (click, drag, press).
// Arguments are (in order):
//  1. Mouse button (e.g. GLFW_MOUSE_BUTTON_LEFT)
//  2. x value on screen, in [0-1] with 0 being left
//  3. y value on screen, in [0-1] with 0 being top
//
using mouse_button_callback = std::function<void(int, double, double)>;

//
// Input is responsible for managing all the input attached to a specific GLFWwindow.
// Often, that means there is one instance of this class for each Engine::Window.
//
// The only reason it's not implemented in Window.cpp is because 1) that would make it super
// lengthy, and 2) it's helpful to understand whether you're interacting with the window itself
// or input in it.
//
class Input
{
private:
   friend void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods);
   friend void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
   friend void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

public:
   //
   // KeyCombination describes a specific set of keypresses, for example Ctrl+S or just Q.
   //
   struct KeyCombination {
      KeyCombination(int key, int mods) : key(key), mods(mods) {};

      int key; // GLFW key code
      int mods; // GLFW modifiers
   };

   static KeyCombination Key(int key) { return KeyCombination{key, 0}; }
   static KeyCombination ShiftKey(int key) { return KeyCombination{key, GLFW_MOD_SHIFT}; }
   static KeyCombination CtrlKey(int key) { return KeyCombination{key, GLFW_MOD_CONTROL}; }
   static KeyCombination AltKey(int key) { return KeyCombination{key, GLFW_MOD_ALT}; }
   static KeyCombination SuperKey(int key) { return KeyCombination{key, GLFW_MOD_SUPER}; }
   static KeyCombination CtrlShiftKey(int key) { return KeyCombination{key, GLFW_MOD_SHIFT | GLFW_MOD_CONTROL}; }

public:
   // Key callbacks are stored as a doubly-linked list, to allow for easy insertion and removal.
   struct KeyCallbackLink {
      int key; // GLFW key code
      int mods; // GLFW modifiers
      input_key_callback callback;

      KeyCallbackLink(Input* manager, const KeyCombination& keyCombination, const input_key_callback& callback)
         : key(keyCombination.key)
         , mods(keyCombination.mods)
         , callback(callback)
         , manager(manager)
         , next(nullptr)
         , prev(nullptr)
      {};
      ~KeyCallbackLink()
      {
         manager->RemoveCallback(this);
      }

   private:
      Input* manager;

      friend class Input;
      friend void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods);
      KeyCallbackLink* next;
      KeyCallbackLink* prev;
   };

public:
   Input();
   ~Input();

public:
   void Initialize(Window* window);

public:
   //
   // Set whether the mouse should be locked to the center of the screen.
   //
   void SetMouseLock(bool locked);
   bool IsMouseLocked() { return mMouseLocked; }

public:
   //
   // Clear the state of all input.
   //
   void Clear();

   //
   // Update the state of input for the last frame.
   //
   void Update();

   //
   // Returns whether or not a specific key is being pressed
   //
   bool IsKeyDown(int key) const;

   //
   // Returns whether or not the mouse is being dragged with a specific button held down.
   //
   bool IsDragging(int button) const { return mMouseDragging[button]; }

   //
   // Get the mouse's current position, in pixel space
   //
   glm::tvec2<double> GetRawMousePosition() const;

   //
   // Get the mouse's current position, in [-1,1] space.
   //
   glm::tvec2<double> GetMousePosition() const;

   //
   // Get the mouse's most recent movement.
   //
   glm::tvec2<double> GetMouseMovement() const { return mMouseMovement; }

   //
   // Get the mouse scroll since last frame.
   //
   glm::tvec2<double> GetMouseScroll() const { return mLastMouseScroll; }

   //
   // Deregister a key's callback.
   //
   void RemoveCallback(std::unique_ptr<KeyCallbackLink> link);

private:
   // The actual work for the above function.
   void RemoveCallback(KeyCallbackLink* link);

public:
   //
   // Register a callback to a specific set of key combinations.
   // Returns a unique_ptr, meaning that once the returned value has been deconstructed
   // the callback will expire. If a callback never seems to trigger, make sure something
   // owns the pointer to it.
   //
   std::unique_ptr<KeyCallbackLink> AddCallback(int key, input_key_callback cb);
   std::unique_ptr<KeyCallbackLink> AddCallback(KeyCombination key, input_key_callback cb);
   std::vector<std::unique_ptr<KeyCallbackLink>> AddCallback(const std::vector<KeyCombination>& keys, input_key_callback cb);

   //
   // Register a callback to mouse events
   //
   void OnMouseDown(mouse_button_callback cb) { mMouseDownCallback = cb; }
   void OnMouseUp(mouse_button_callback cb) { mMouseUpCallback = cb; }
   void OnClick(mouse_button_callback cb) { mMouseClickCallback = cb; }

private:
   // They might as well be one class, but it would be a very large unwieldy class.
   friend class Window;
   Window* mWindow;

   // The "key-ring" for callbacks, indexed by key.
   KeyCallbackLink* mKeyCallbacks[GLFW_KEY_LAST] = {nullptr};
   
   bool mMouseLocked = false;

   mouse_button_callback mMouseDownCallback = nullptr;
   mouse_button_callback mMouseUpCallback = nullptr;
   mouse_button_callback mMouseClickCallback = nullptr;

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

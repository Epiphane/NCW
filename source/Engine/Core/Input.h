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
// Input is a generic base class responsible for "Input"-y things: mouse movement, clicking,
// keyboard interaction, etc. It's implemented primarily by Engine::Window, but defined as
// extensible so that the Editor can override this and create a shim input to send to its sub-
// states.
//
class Input
{
public:
   //
   // KeyCombination describes a specific set of keypresses, for example Ctrl+S or just Q.
   //
   struct KeyCombination {
      KeyCombination(int key, int mods) : key(key), mods(mods) {};

      int key; // key code
      int mods; // modifiers
   };

public:
   // Key callbacks are stored as a doubly-linked list, to allow for easy insertion and removal.
   struct KeyCallbackLink {
      int key; // key code
      int mods; // modifiers
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
   virtual ~Input();

public:
   //
   // Reset the state of all input.
   //
   virtual void Reset();

   //
   // Update the state of input for the last frame.
   //
   virtual void Update() {}

   //
   // Returns whether or not a specific key is being pressed
   //
   virtual bool IsKeyDown(int key) const = 0;

   //
   // Returns whether or not a specific mouse button is being pressed
   //
   virtual bool IsMouseDown(int button) const = 0;

   //
   // Returns whether or not the mouse is being dragged with a specific button held down.
   //
   virtual bool IsDragging(int button) const = 0;

   //
   // Get the mouse's current position, in pixel space
   //
   virtual glm::tvec2<double> GetRawMousePosition() const = 0;

   //
   // Get the mouse's current position, in [-1,1] space.
   //
   virtual glm::tvec2<double> GetMousePosition() const = 0;

   //
   // Get the mouse's most recent movement.
   //
   virtual glm::tvec2<double> GetMouseMovement() const = 0;

   //
   // Get the mouse scroll since last frame.
   //
   virtual glm::tvec2<double> GetMouseScroll() const = 0;

   //
   // Set whether the mouse should be locked to the center of the screen.
   //
   virtual void SetMouseLock(bool locked) = 0;

   //
   // Returns whether the mouse is currently locked to the center of the screen.
   //
   virtual bool IsMouseLocked() const = 0;

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

protected:
   //
   // Trigger an event through the entire KeyCallback ring (to be called by subclasses)
   //
   void TriggerKeyCallbacks(int key, int action, int mods);

private:
   // The "key-ring" for callbacks, indexed by key.
   KeyCallbackLink* mKeyCallbacks[GLFW_KEY_LAST] = {nullptr};
   
protected:
   mouse_button_callback mMouseDownCallback = nullptr;
   mouse_button_callback mMouseUpCallback = nullptr;
   mouse_button_callback mMouseClickCallback = nullptr;
};

}; // namespace Engine

}; // namespace CubeWorld

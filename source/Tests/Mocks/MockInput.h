// By Thomas Steinke

#pragma once

#include <Engine/Core/Input.h>

namespace CubeWorld
{

namespace Test
{

// TODO Implement a "GL stub" that fakes all of the OpenGL calls,
// so that tests involving graphics resources can be run without
// spinning up real graphical contexts. In the meantime, hide all
// tests with graphical tie-ins
#define GL_TEST_FLAG "[.] [OpenGL]"

class MockInput : public Engine::Input
{
public:
   MockInput();
   virtual ~MockInput();

public:
   //
   // Functions for setting input state.
   //
   void SetMousePosition(glm::tvec2<double> pos);
   void MoveMouse(glm::tvec2<double> amount);
   void MouseDown(int button);
   void MouseUp(int button);
   void Click(int button, glm::tvec2<double> pos);

   void SetScroll(glm::tvec2<double> amount);

   void KeyDown(int key, int mods = 0);
   void KeyUp(int key, int mods = 0);
   void Press(int key, int mods = 0);

   void Type(unsigned int codePoint);
   void Type(const std::string& str);

public:
   //
   // Overrides from Input base class.
   //
   void Reset() override;
   void Update() override;
   bool IsKeyDown(int key) const override { return mKeyPressed[key]; }
   bool IsMouseDown(int button) const override { return mMousePressed[button]; }
   bool IsDragging(int button) const override { return mMouseDragging[button]; }
   glm::tvec2<double> GetRawMousePosition() const override { return mMousePosition; }
   glm::tvec2<double> GetMousePosition() const override { return mMousePosition; }
   glm::tvec2<double> CorrectYCoordinate(glm::tvec2<double> position) const override { return position; }
   glm::tvec2<double> GetMouseMovement() const override { return mMouseMovement; }
   glm::tvec2<double> GetMouseScroll() const override { return mLastMouseScroll; }
   void SetMouseLock(bool locked) override { mMouseLocked = locked; }
   bool IsMouseLocked() const override { return mMouseLocked; }

private:
   bool mMouseLocked = false;

   glm::tvec2<double> mMousePosition;
   glm::tvec2<double> mMouseMovement;

   glm::tvec2<double> mLastMouseScroll;

   bool mKeyPressed[GLFW_KEY_LAST];
   bool mMousePressed[GLFW_MOUSE_BUTTON_LAST];
   bool mMouseDragging[GLFW_MOUSE_BUTTON_LAST];
   glm::tvec2<double> mMousePressOrigin[GLFW_MOUSE_BUTTON_LAST];
};

}; // namespace Test

}; // namespace CubeWorld

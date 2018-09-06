// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Event.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>

namespace CubeWorld
{

namespace Editor
{

//
// Controls manages the UI that goes in the toolbar for the editor.
//
// It should be one persistent instance, that updates depending on the
// current state of the editor (e.g. AnimationStation vs something else).
//
class Controls {
public:
   // Describes the layout of the Controls Panel
   struct Layout {
      struct Element {
         Element(const std::string& name) : name(name) {};

         std::string name;
      };

      Layout(const std::vector<Element>& elements) : elements(elements) {};

      std::vector<Element> elements;
   };

public:
   Controls(Engine::Window* window);
   ~Controls();

   //
   // Check input, issue events, and rebuild the window.
   //
   void Update();

   //
   // Rebuild the UI.
   //
   void Rebuild(const Layout& layout);

private:
   Engine::Graphics::CameraHandle mCamera;

   Engine::Window* mWindow;
   std::unique_ptr<Engine::Input::InputManager> mInput;
};

}; // namespace Editor

}; // namespace CubeWorld

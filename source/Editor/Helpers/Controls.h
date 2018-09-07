// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Input.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/VBO.h>

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
   Controls(Engine::Window* window, Engine::Input::InputManager* input);
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
   Engine::Window* mWindow;
   Engine::Input::InputManager* mInput;

   std::unique_ptr<Engine::Graphics::Font> mFont;
   Engine::Graphics::VBO mControlsTextVBO;
   GLint mControlsTextCount;

private:
   static GLuint program;
   static GLuint aPosition, aUV;
   static GLuint uTexture, uWindowSize;
};

}; // namespace Editor

}; // namespace CubeWorld

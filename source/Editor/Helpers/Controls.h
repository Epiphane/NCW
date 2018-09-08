// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Bounded.h>
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
   Controls(Bounded* bounds);
   ~Controls();

   //
   // Check input, issue events, and rebuild the window.
   //
   void Update();

   //
   // Rebuild the UI.
   //
   void Rebuild(const Layout& layout);

   //
   // React to mouse events
   //
   void MouseDown(int button, double x, double y);
   void MouseUp(int button, double x, double y);
   void MouseClick(int button, double x, double y);
   void MouseDrag(int button, double x, double y);

private:
   Bounded* mBounds;

   Layout mLayout;

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

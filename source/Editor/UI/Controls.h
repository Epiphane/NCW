// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/VBO.h>

#include "../UI/SubWindow.h"

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
class Controls : public SubWindow {
public:
   // Describes the layout of the Controls Panel
   struct Layout {
      struct Element {
         using Callback = std::function<void(void)>;

         std::string label;
         Callback callback;
      };

      Layout(const std::vector<Element>& elements) : elements(elements) {};

      std::vector<Element> elements;
   };

public:
   Controls(
      Bounded& parent,
      const Options& options
   );

   //
   // Rebuild the UI.
   //
   void SetLayout(const Layout& layout);

protected:
   void Rebuild();

private:
   Layout mLayout;
};

}; // namespace Editor

}; // namespace CubeWorld

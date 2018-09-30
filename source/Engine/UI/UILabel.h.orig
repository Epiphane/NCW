//
// UILabel - Render a piece of text in the UI
//
// By Thomas Steinke + Elliot Fiske
//

#pragma once

#include "UIElement.h"

#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/VBO.h>

namespace CubeWorld
{

namespace Engine
{
   
class UILabel : UIElement {
public:
   UILabel();
   
private:
   std::unique_ptr<Engine::Graphics::Font> mpFont;
   
   static GLuint program;
   static GLuint aPosition;
   static GLuint uWindowSize, uColor;
};

   
}; // namespace Engine

}; // namespace CubeWorld

//
// UILabel - Render a piece of text in the UI
//
// By Thomas Steinke
//

#pragma once

#include <string>

#include <Engine/Core/Window.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/VBO.h>

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class UILabel : UIElement {
public:
   UILabel(UIRoot* root, UIElement* parent);
      
   virtual void   AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices) override;
   virtual size_t Render(Engine::Graphics::VBO& vbo, size_t offset) override;
      
   void SetText(const std::string& text);
   std::string GetText();
      
   void SetTextColor(float r, float g, float b, float a);
   glm::vec4 GetTextColor();
      
   void SetUsesInnateSizeConstraint(bool bUseHorizontalSizeConstraint, bool bUseVerticalSizeConstraint);
      
private:
   std::string mText;
      
   glm::vec4 mTextColor;
   Engine::Graphics::Font* mpFont;
      
   rhea::constraint mInnateHorizontalSize;
   rhea::constraint mInnateVerticalSize;
      
   static GLuint program;
   static GLuint aPosition;
   static GLuint uWindowSize, uColor;
};

}; // namespace Engine

}; // namespace CubeWorld

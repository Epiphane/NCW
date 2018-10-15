//
// UILabel - Render a piece of text in the UI
//
// By Thomas Steinke
//

#pragma once

#include <string>

#include <Engine/Aggregator/Text.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/VBO.h>

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class UILabel : public UIElement {
public:
   UILabel(UIRoot* root, UIElement* parent);
      
   void SetText(const std::string& text);
   std::string GetText();
      
   void SetTextColor(float r, float g, float b, float a);
   glm::vec4 GetTextColor();
   
   void Receive(const Engine::UIRebalancedEvent& evt) override;
      
private:
   void RenderText();
   
   std::string mText;
      
   glm::vec4 mTextColor;
   Engine::Graphics::Font* mpFont;
    
   Engine::Aggregator::Text::Region mRegion;
};

}; // namespace Engine

}; // namespace CubeWorld

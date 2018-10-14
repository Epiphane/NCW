//
// RectFilled – render a solid rectangle with a given color in the UI.
//
// By Thomas Steinke
//

#pragma once

#include "../Aggregator/Rect.h"

#include <Engine/Graphics/Program.h>
#include <Engine/UI/UIElement.h>

namespace CubeWorld
{

namespace UI
{
   
class RectFilled : public Engine::UIElement {
public:
   RectFilled(Engine::UIRoot* root, UIElement* parent, glm::vec4 color = glm::vec4(0, 0, 0, 1));

   //
   // Set the fill color of this rectangle. r, g, b, and a are between 0 and 1.
   //
   void SetColor(glm::vec4 color);

   //
   // Get the current fill color of this rectangle.
   //
   glm::vec4 GetColor() { return mColor; }

public:
   void Receive(const Engine::UIRebalancedEvent& evt) override;

private:
   void Update();

private:
   glm::vec4 mColor;

private:
   Aggregator::Rect::Region mRegion;
};

   
}; // namespace UI

}; // namespace CubeWorld

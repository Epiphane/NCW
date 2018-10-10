//
// UIRectFilled – render a solid rectangle with a given color in the UI.
//
// By Thomas Steinke
//

#pragma once

#include <Engine/Aggregator/Rect.h>

#include "../Graphics/Program.h"
#include "../Aggregator/Rect.h"
#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{
   
class UIRectFilled : public UIElement {
public:
   UIRectFilled(UIRoot* root, UIElement* parent);

   //
   // Set the fill color of this rectangle. r, g, b, and a are between 0 and 1.
   //
   void SetColor(glm::vec4 color);

   //
   // Get the current fill color of this rectangle.
   //
   glm::vec4 GetColor() { return mColor; }

public:
   void Receive(const UIRebalancedEvent& evt) override;

private:
   void Update();

private:
   glm::vec4 mColor;

private:
   Engine::Aggregator::Rect::Region mRegion;

   static std::unique_ptr<Engine::Graphics::Program> program;
};

   
}; // namespace Engine

}; // namespace CubeWorld

// By Thomas Steinke

#include <glm/glm.hpp>

#include "../Core/Window.h"
#include "../Logger/Logger.h"

#include "UIRoot.h"
#include "UIRectFilled.h"

namespace CubeWorld
{

namespace Engine
{

UIRectFilled::UIRectFilled(UIRoot* root, UIElement* parent, glm::vec4 color)
   : UIElement(root, parent)
   , mColor(color)
   , mRegion(root->Reserve<Engine::Aggregator::Rect>(2))
{
}

void UIRectFilled::Update()
{
   std::vector<Engine::Aggregator::RectData> data({
      { mFrame.GetBottomLeft(), mColor },
      { mFrame.GetTopRight(), mColor },
   });

   mRegion.Set(data);
}

void UIRectFilled::SetColor(glm::vec4 color)
{
   mColor = color;
   Update();
}

void UIRectFilled::Receive(const Engine::UIRebalancedEvent& evt)
{
   UIElement::Receive(evt);
   Update();
}

}; // namespace Engine

}; // namespace CubeWorld

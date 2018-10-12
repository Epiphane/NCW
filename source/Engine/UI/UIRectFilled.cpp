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

UIRectFilled::UIRectFilled(UIRoot* root, UIElement* parent)
   : UIElement(root, parent)
   , mColor(1, 1, 1, 1)
   , mRegion(root->Reserve<Engine::Aggregator::Rect>(2))
{
}

void UIRectFilled::Update()
{
   std::vector<Engine::Aggregator::RectData> data({
      {
         glm::vec2(mFrame.left.int_value(), mFrame.bottom.int_value()),
         mColor
      },
      {
         glm::vec2(mFrame.right.int_value(), mFrame.top.int_value()),
         mColor
      },
   });

   mRegion.Set(data);
}

void UIRectFilled::SetColor(glm::vec4 color)
{
   mColor = color;
   Update();
}

void UIRectFilled::Receive(const Engine::UIRebalancedEvent&)
{
   Update();
}

}; // namespace Engine

}; // namespace CubeWorld

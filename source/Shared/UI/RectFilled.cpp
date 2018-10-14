// By Thomas Steinke

#include <glm/glm.hpp>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "RectFilled.h"

namespace CubeWorld
{

namespace UI
{

RectFilled::RectFilled(Engine::UIRoot* root, UIElement* parent, glm::vec4 color)
   : UIElement(root, parent)
   , mColor(color)
   , mRegion(root->Reserve<Aggregator::Rect>(2))
{
}

void RectFilled::Update()
{
   std::vector<Aggregator::RectData> data({
      { mFrame.GetBottomLeft(), mColor },
      { mFrame.GetTopRight(), mColor },
   });

   mRegion.Set(data);
}

void RectFilled::SetColor(glm::vec4 color)
{
   mColor = color;
   Update();
}

void RectFilled::Receive(const Engine::UIRebalancedEvent& evt)
{
   UIElement::Receive(evt);
   Update();
}

}; // namespace UI

}; // namespace CubeWorld

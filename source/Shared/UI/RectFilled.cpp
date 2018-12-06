// By Thomas Steinke

#include <glm/glm.hpp>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include <Shared/Helpers/JsonHelper.h>

#include "RectFilled.h"

namespace CubeWorld
{

namespace UI
{

RectFilled::RectFilled(Engine::UIRoot* root, UIElement* parent, const std::string& name, glm::vec4 color)
   : UIElement(root, parent, name)
   , mColor(color)
   , mRegion(root->Reserve<Aggregator::Rect>(2))
{
}

void RectFilled::Redraw()
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
   Redraw();
}

void RectFilled::InitFromJSON(nlohmann::json data)
{
   UIElement::InitFromJSON(data);

   mColor = Shared::JsonHelpers::JsonToVec4(data["backgroundColor"]);
}

}; // namespace UI

}; // namespace CubeWorld

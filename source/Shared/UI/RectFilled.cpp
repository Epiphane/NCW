// By Thomas Steinke

#include <glm/glm.hpp>

#include <RGBLogger/Logger.h>
#include <Engine/Core/Window.h>
#include <Engine/UI/UIRoot.h>

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
   
Engine::UIElement::DebugInfo RectFilled::GetDebugInfo(bool bRecursive) {
   DebugInfo result = UIElement::GetDebugInfo(bRecursive);
   
   result.type = "RectFilled";
   
   return result;
}

void RectFilled::Redraw()
{
   std::vector<Aggregator::RectData> data;
   
   if (mActive) {
      data = {
         { mFrame.GetBottomLeft(), mColor },
         { mFrame.GetTopRight(), mColor },
      }; 
   }
   
   while (data.size() < mRegion.size())
   {
      data.push_back(Aggregator::RectData{glm::vec3(0),glm::vec4(0)});
   }

   mRegion.Set(data.data());
}

void RectFilled::SetColor(glm::vec4 color)
{
   mColor = color;
   Redraw();
}

void RectFilled::InitFromJSON(const BindingProperty& data)
{
   UIElement::InitFromJSON(data);

   mColor = data["backgroundColor"].GetVec4();
}

}; // namespace UI

}; // namespace CubeWorld

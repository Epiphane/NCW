// By Thomas Steinke

#include <glm/glm.hpp>

#include <RGBLogger/Logger.h>
#include <Engine/Core/Window.h>
#include <Engine/UI/UIRootDep.h>

#include "RectFilled.h"

namespace CubeWorld
{

namespace UI
{

RectFilled::RectFilled(Engine::UIRootDep* root, UIElementDep* parent, const std::string& name, glm::vec4 color)
   : UIElementDep(root, parent, name)
   , mColor(color)
   , mRegion(root->Reserve<Aggregator::Rect>(2))
{
}

Engine::UIElementDep::DebugInfo RectFilled::GetDebugInfo(bool bRecursive) {
   DebugInfo result = UIElementDep::GetDebugInfo(bRecursive);

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
   UIElementDep::InitFromJSON(data);

   mColor = data["backgroundColor"].GetVec4();
}

}; // namespace UI

}; // namespace CubeWorld

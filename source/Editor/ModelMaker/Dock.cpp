// By Thomas Steinke

#include <Shared/Helpers/Asset.h>

#include "../Command/CommandStack.h"
#include <Shared/UI/Button.h>
#include <Shared/UI/Image.h>
#include <Shared/UI/RectFilled.h>

#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

using Engine::UIElement;
using Engine::UIFrame;
using UI::Button;
using UI::RectFilled;

Dock::Dock(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent)
{
   // Background
   {
      RectFilled* bg = Add<RectFilled>(glm::vec4(0.2, 0.2, 0.2, 1));
      RectFilled* fg = Add<RectFilled>(glm::vec4(0, 0, 0, 1));

      UIFrame& fBackground = bg->GetFrame();
      UIFrame& fForeground = fg->GetFrame();
      root->AddConstraints({
         fBackground.left == mFrame.left,
         fBackground.right == mFrame.right,
         fBackground.top == mFrame.top,
         fBackground.bottom == mFrame.bottom,
         mFrame > fForeground,

         fForeground.left == fBackground.left,
         fForeground.right == fBackground.right - 2,
         fForeground.top == fBackground.top - 2,
         fForeground.bottom == fBackground.bottom + 2,
         fForeground > fBackground,
      });
   }

   root->Subscribe<Engine::ComponentAddedEvent<CubeModel>>(*this);
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<CubeModel>&)
{
}

///
///
///
void Dock::Update(TIMEDELTA dt)
{
   UIElement::Update(dt);
}

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld

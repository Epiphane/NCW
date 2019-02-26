//
// ElementListItem.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ElementListItem.h"

#include <Shared/Helpers/Asset.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using UI::Image;
using UI::Text;

ElementListItem::ElementListItem(Engine::UIRoot* root, Engine::UIElement* parent, const std::string &name)
      : UIElement(root, parent, name)
{
   mArrow = Add<Image>(Image::Options{Asset::Image("EditorIcons.png"), "button_right"});
   mLabel = Add<Text>(Text::Options{"Funny man"});
   
   mLabel->ConstrainWidth(100);
   mLabel->ConstrainHeight(20);

   mArrow->ConstrainWidth(10);
   
   mLabel->ConstrainToRightOf(mArrow, 5.0);
   mLabel->ConstrainVerticalCenterTo(mArrow);
   
   this->Contains(mLabel);
   this->Contains(mArrow);
   
   Engine::UIConstraint::Options opt;
   opt.relationship = Engine::UIConstraint::GreaterThanOrEqual;
   this->ConstrainHeight(0, opt);
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

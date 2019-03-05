//
// ElementListSidebar.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ElementListSidebar.h"

#include "ElementListItem.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

ElementListSidebar::ElementListSidebar(Engine::UIRoot* root, Engine::UIElement* parent)
      : UIElement(root, parent, "ConstrainerElementList")
{
   mStackView = Add<UIStackView>("ElementListView");
   mStackView->SetAlignItemsBy(UIStackView::Left);
   
   this->Contains(mStackView);
   mStackView->ConstrainTopAlignedTo(this);

   mStackView->Add<ElementListItem>("Test1");
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

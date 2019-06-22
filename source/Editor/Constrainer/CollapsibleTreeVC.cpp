//
// CollapsibleTreeVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleTreeVC.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

CollapsibleTreeVC::CollapsibleTreeVC(Engine::UIRoot* root, Engine::UIElement* parent, 
                                     const std::string& name)
   : UIElement(root, parent, name)
{
   mStackView = Add<UIStackView>("ElementListView");
   mStackView->SetAlignItemsBy(UIStackView::Left);
   
   mStackView->ConstrainTopAlignedTo(this);
   mStackView->ConstrainHorizontalCenterTo(this);
   mStackView->ConstrainWidthTo(this);
   
//   DataChanged();
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

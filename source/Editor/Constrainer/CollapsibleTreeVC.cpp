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
   
   DataChanged();
}

void CollapsibleTreeVC::DataChanged()
{
   for (auto it = mStackView->BeginChildren(); it != mStackView->EndChildren(); it++) {
      it->MarkForDeletion();
   }
   
   if (!mDatasource) {
      return;
   }
   
   for (uint32_t ndx = 0; ndx < mDatasource->NumberOfCellsInTableView(); ndx++) {
      std::unique_ptr<CollapsibleTreeItem> newItem = mDatasource->GetTreeItemAtIndex(ndx);
      mStackView->AddChild(std::move(newItem));
   }
}

void CollapsibleTreeVC::ItemWasClicked(CollapsibleTreeItem* item)
{
   if (mDelegate) {
      mDelegate->ItemSelected(item);
   }
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

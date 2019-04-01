//
// CollapsibleTreeView.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleTreeView.h"

#include "CollapsibleTreeItem.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

CollapsibleTreeView::CollapsibleTreeView(Engine::UIRoot* root, Engine::UIElement* parent, 
                                         CollapsibleTreeViewDelegate* delegate, CollapsibleTreeViewDatasource* datasource)
   : UIElement(root, parent, "ConstrainerElementList")
   , mDelegate(delegate)
   , mDatasource(datasource)
{
   mStackView = Add<UIStackView>("ElementListView");
   mStackView->SetAlignItemsBy(UIStackView::Left);
   
   mStackView->ConstrainTopAlignedTo(this);
   mStackView->ConstrainHorizontalCenterTo(this);
   mStackView->ConstrainWidthTo(this);
   
   DataChanged();
}

void CollapsibleTreeView::DataChanged()
{
   for (auto it = mStackView->BeginChildren(); it != mStackView->EndChildren(); it++) {
      it->MarkForDeletion();
   }
   
   if (!mDatasource) {
      return;
   }
   
   for (uint32_t ndx = 0; ndx < mDatasource->NumberOfCellsInTableView(); ndx++) {
      std::unique_ptr<CollapsibleTreeItem> newItem = mDatasource->GetTreeItemAtIndex(ndx);
      newItem->SetTreeView(this);
      mStackView->AddChild(std::move(newItem));
   }
}

void CollapsibleTreeView::ItemWasClicked(CollapsibleTreeItem* item)
{
   if (mDelegate) {
      mDelegate->ItemSelected(item);
   }
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

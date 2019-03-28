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
   
void CollapsibleTreeView::CreateItemForData(const CollapsibleTreeItemData& /*data*/) {
   
}   

void CollapsibleTreeView::DataChanged()
{
   for (auto it = mStackView->BeginChildren(); it != mStackView->EndChildren(); it++) {
      it->MarkForDeletion();
   }
   
   if (!mDatasource) {
      return;
   }
   
   for (uint32_t ndx = 0; ndx < mDatasource->NumberOfRootElementsForTree(); ndx++) {
      std::unique_ptr<CollapsibleTreeItem> newItem = mDatasource->GetTreeItemAtIndex(ndx);
      mStackView->AddChild(std::move(newItem));
   }
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

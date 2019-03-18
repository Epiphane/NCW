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
   
   DataChanged();
}
   
void CollapsibleTreeView::CreateItemForData(const CollapsibleTreeItemData& data) {
   
}   

void CollapsibleTreeView::DataChanged()
{
   for (auto it = mStackView->BeginChildren(); it != mStackView->EndChildren(); it++) {
      it->MarkForDeletion();
   }
   
   if (!mDatasource) {
      return;
   }
   
   std::unique_ptr<CollapsibleTreeItemData> newData = mDatasource->GetTreeData();
   mStackView->Add<CollapsibleTreeItem>(newData->title);
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

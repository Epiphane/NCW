//
// CollapsibleTreeView.h
//
// UIElement that holds a tree of items. You can click each
//    item to expand the children underneath it in the tree.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIRoot.h>
#include <Engine/UI/UIStackView.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

class CollapsibleTreeItem; ///< Forward declare
   
using Engine::UIStackView;
using Engine::UIElement;

struct CollapsibleTreeItemData {
   std::string uniqueId;
   std::string title;
   std::vector<CollapsibleTreeItemData> children;
};
   
class CollapsibleTreeViewDatasource {
public:
   virtual uint32_t NumberOfRootElementsForTree() = 0;
   virtual std::unique_ptr<CollapsibleTreeItem> GetTreeItemAtIndex(uint32_t index) = 0;
};
   
class CollapsibleTreeViewDelegate {
public:
   // Called when the user selects a tree item
   virtual void ItemSelected(CollapsibleTreeItem* item) = 0;
};

class CollapsibleTreeView : public UIElement
{
public:
   CollapsibleTreeView(Engine::UIRoot* root, UIElement* parent, CollapsibleTreeViewDelegate* delegate, CollapsibleTreeViewDatasource* datasource);

   // When called, I will ask my datasource for the new tree views
   void DataChanged();
   
private:
   // Helper function that creates + adds a CollapsibleTreeItem for the given data
   void CreateItemForData(const CollapsibleTreeItemData& data);
   
   CollapsibleTreeViewDelegate*   mDelegate;
   CollapsibleTreeViewDatasource* mDatasource;
   
   UIStackView *mStackView;

   // List of items in the tree view. All of these items are direct children of mStackView.
   std::vector<CollapsibleTreeItem*> mItems;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

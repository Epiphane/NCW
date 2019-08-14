//
// CollapsibleTreeVC.h
//
// UIElement that holds a tree of items. You can click each
//    item to expand the children underneath it in the tree.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIRoot.h>
#include <Engine/UI/UIStackView.h>

#include "CollapsibleTreeItem.h"

namespace CubeWorld
{

namespace Engine
{
   
using Engine::CollapsibleTreeItem;

class CollapsibleTreeVC : public UIElement
{
public:
   CollapsibleTreeVC(Engine::UIRoot* root, UIElement* parent, const std::string& name);
   
   Observables::Observable<std::vector<CollapsibleTreeItem::Data>>& GetDataSink() { return mDataInputObserver; }
   const std::vector<CollapsibleTreeItem*>& GetRootItems() const { return mRootItems; }

private:
   // The root-level items for this tree
   std::vector<CollapsibleTreeItem*> mRootItems;
   
   UIStackView* mRootItemStackView;
   
   // Send a message into this and I will react by making a new set of CollapsibleTreeItems to match the message's heirarchy
   Observables::Observable<std::vector<CollapsibleTreeItem::Data>> mDataInputObserver;
};

}; // namespace Engine

}; // namespace CubeWorld

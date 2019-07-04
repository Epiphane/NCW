//
// CollapsibleTreeVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleTreeVC.h"

#include <RGBBinding/ObservableBasicOperations.h>

namespace CubeWorld
{

namespace Engine
{

namespace UI
{
   
using namespace Observables;

CollapsibleTreeVC::CollapsibleTreeVC(Engine::UIRoot* root, Engine::UIElement* parent, const std::string& name)
   : UIElement(root, parent, name)
{
   mDataInputObserver >>
      OnMessage<std::vector<CollapsibleTreeItem::Data>>([&](std::vector<CollapsibleTreeItem::Data> newData) {
         for (auto oldRootItem : mRootItems) {
            oldRootItem->MarkForDeletion();
         }
         
         mRootItems.clear();
         
         for (CollapsibleTreeItem::Data itemData : newData) {
            auto newItem = Add<CollapsibleTreeItem>();
            newItem->GetDataSink().SendMessage(itemData);
            mRootItems.push_back(newItem);
         }
      }, mBag);
}

}; // namespace Engine

}; // namespace UI

}; // namespace CubeWorld

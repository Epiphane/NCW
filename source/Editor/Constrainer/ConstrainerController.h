// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/StateWindow.h>
#include <Shared/Helpers/JsonFileSync.h>

#include "ConstrainerModel.h"

#include "CollapsibleTreeView.h"

#include "../Controls.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

class ConstrainerController : public Engine::UIRoot, public CollapsibleTreeViewDatasource, public CollapsibleTreeViewDelegate
{
public:
   ConstrainerController(Engine::Input* input, const Controls::Options& options);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

   // Collapsible Tree View Datasource
   std::unique_ptr<CollapsibleTreeItemData> GetTreeData() override;

   // Collapsible Tree View Delegate
   void ItemSelected(CollapsibleTreeItem* item) override;


private:
   // Called when the Model has updates (i.e. if the user edits UI, or if the underlying JSON changes)
   void ModelUpdated();
   std::unique_ptr<CollapsibleTreeItemData> ParseUIElementTitles(UIElement& baseElement);

   Engine::EventManager mEvents;

   ConstrainerModel mModel;
   
   CollapsibleTreeView *mElementList;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

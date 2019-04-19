// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/StateWindow.h>
#include <Shared/Helpers/JsonFileSync.h>

#include "ConstrainerModel.h"

#include "CollapsibleTreeVC.h"

#include "../Controls.h"

#include "../UI/ReactiveX/linq.hpp"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

class ConstrainerVC : public Engine::UIRoot, public CollapsibleTreeViewDatasource, public CollapsibleTreeViewDelegate
{
public:
   ConstrainerVC(Engine::Input* input, const Controls::Options& options);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

   // Collapsible Tree View Datasource
   uint32_t NumberOfCellsInTableView() override;
   std::unique_ptr<CollapsibleTreeItem> GetTreeItemAtIndex(uint32_t index) override;

   // Collapsible Tree View Delegate
   void ItemSelected(CollapsibleTreeItem* item) override;


private:
   // Called when the Model has updates (i.e. if the user edits UI, or if the underlying JSON changes)
   void ModelUpdated();
   std::unique_ptr<CollapsibleTreeItem> ParseUIElementTitles(UIElement& baseElement);

   Engine::EventManager mEvents;

   ConstrainerModel mModel;
   
   CollapsibleTreeVC *mElementList;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

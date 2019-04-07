// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <RGBLogger/Logger.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Engine/UI/UIContextMenu.h>
#include <Engine/UI/UISerializationHelper.h>

#include "CollapsibleTreeItem.h"
#include "ConstrainerController.h"
#include "Sidebar.h"
#include "UIElementDesignerWrapper.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::UIContextMenu;
using UI::RectFilled;

ConstrainerController::ConstrainerController(Engine::Input* input, const Controls::Options& options)
   : UIRoot(input)
{
   mElementList = Add<CollapsibleTreeView>(this, this);

   Sidebar* sidebar = Add<Sidebar>();
   Controls* controls = Add<Controls>(options);

   sidebar->ConstrainLeftAlignedTo(this);
   sidebar->ConstrainTopAlignedTo(this);
   sidebar->ConstrainWidthTo(this, 0.0, 0.2);
   sidebar->ConstrainAbove(controls);

   controls->ConstrainLeftAlignedTo(this);
   controls->ConstrainBottomAlignedTo(this);
   controls->ConstrainWidthTo(sidebar);
   
   mElementList->ConstrainToRightOf(sidebar);
   mElementList->ConstrainWidthTo(this, 0.0, 0.2);
   mElementList->ConstrainTopAlignedTo(this);
   mElementList->ConstrainBottomAlignedTo(this);

   Engine::UISerializationHelper serializer;

   std::string path = Paths::Normalize(Asset::UIElement("example_ui_serialized.json"));
   Maybe<Engine::ElementsByName> maybeElementMap = serializer.CreateUIFromJSONFile(path, mpRoot, mContentLayer);

   Engine::ElementsByName elementMap = *maybeElementMap;

   UIElement* mainContent = elementMap["TestJSONStuff"];
   mainContent->ConstrainHeightTo(this);
   mainContent->ConstrainToRightOf(mElementList);
   mainContent->ConstrainTopAlignedTo(this);
   mainContent->ConstrainRightAlignedTo(this);

   mModel.SetModelUpdatedCallback(std::bind(&ConstrainerController::ModelUpdated, this));
   mModel.SetBaseElement(mainContent);

   UIElement* wrapperLayer = Add<UIElement>();
   wrapperLayer->ConstrainInFrontOfAllDescendants(mainContent);

   for (auto elementPair : elementMap) {
      UIElement* elementToWrap = elementPair.second;
      std::string wrapperName = elementToWrap->GetName();
      wrapperName.append("_DesignWrapper");

      wrapperLayer->Add<UIElementDesignerWrapper>(wrapperName, elementToWrap);
   }
}

void ConstrainerController::ModelUpdated()
{
   // Convert the element tree into just their names and pass it
   //    through to the CollapsibleTreeView
   mElementList->DataChanged();
}

std::unique_ptr<CollapsibleTreeItem> ConstrainerController::ParseUIElementTitles(UIElement& baseElement)
{
   auto result = std::make_unique<CollapsibleTreeItem>(mpRoot, this, "", baseElement.GetName());
   
   for (auto it = baseElement.BeginChildren(); it != baseElement.EndChildren(); it++) {
      result->AddSubElement(ParseUIElementTitles(*it));
   }
   
   return result;
}

void ConstrainerController::Start()
{
}

#pragma mark - Collapsible Tree View Datasource

std::unique_ptr<CollapsibleTreeItem> ConstrainerController::GetTreeItemAtIndex(uint32_t /*index*/)
{
   return ParseUIElementTitles(*mModel.GetBaseElement());
}

#pragma mark - Collapsible Tree View Delegate

void ConstrainerController::ItemSelected(CollapsibleTreeItem* /*item*/)
{
   printf("Selected an item!");
}

uint32_t ConstrainerController::NumberOfRootElementsForTree()
{
   if (!mModel.GetBaseElement()) {
      return 0;
   }
   return 1;
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

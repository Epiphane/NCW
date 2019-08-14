// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <RGBLogger/Logger.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Engine/UI/UIContextMenu.h>
#include <Engine/UI/UISerializationHelper.h>
#include <Engine/UI/CollapsibleTreeItem.h>
#include <RGBBinding/ObservableBasicOperations.h>

#include "ConstrainerVC.h"
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

using TreeData = Engine::CollapsibleTreeItem::Data;   

ConstrainerVC::ConstrainerVC(Engine::Input* input, const Controls::Options& options)
   : UIRoot(input)
{
   mElementList = Add<CollapsibleTreeVC>("ElementList");

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
   mElementList->ConstrainTopAlignedTo(this, 300);
//   mElementList->ConstrainBottomAlignedTo(this);

   std::function<TreeData(UIElement*)> elementToTreeData = [&](UIElement* baseElement) -> TreeData {
      TreeData datum;
      
      datum.title = baseElement->GetName();
      for (const auto& child : baseElement->GetChildren()) {
         datum.children.push_back(elementToTreeData(child.get()));
      }
      
      return datum;
   };
   
   mModel.GetBaseElementObservable() >>
      Observables::Map<UIElement*, std::vector<TreeData>>([&](UIElement* newBaseElement) {
         std::vector<TreeData> result = { elementToTreeData(newBaseElement) }; 
         return result;
      }) >>
      mElementList->GetDataSink();
   
   Engine::UISerializationHelper serializer;

   std::string path = Paths::Normalize(Asset::UIElement("example_ui_serialized.json"));
   Maybe<Engine::ElementsByName> maybeElementMap = serializer.CreateUIFromJSONFile(path, mpRoot, mContentLayer);

   Engine::ElementsByName elementMap = *maybeElementMap;

   UIElement* mainContent = elementMap["TestJSONStuff"];
   mainContent->ConstrainHeightTo(this);
   mainContent->ConstrainToRightOf(mElementList);
   mainContent->ConstrainTopAlignedTo(this);
   mainContent->ConstrainRightAlignedTo(this);
   
   mModel.GetBaseElementObservable().SendMessage(mainContent);
   

   UIElement* wrapperLayer = Add<UIElement>();
   wrapperLayer->ConstrainInFrontOfAllDescendants(mainContent);

//   for (auto elementPair : elementMap) {
//      UIElement* elementToWrap = elementPair.second;
//      std::string wrapperName = elementToWrap->GetName();
//      wrapperName.append("_DesignWrapper");
//
//      wrapperLayer->Add<UIElementDesignerWrapper>(wrapperName, elementToWrap);
//   }
}

//std::unique_ptr<CollapsibleTreeItem> ConstrainerVC::ParseUIElementTitles(UIElement& baseElement)
//{
//   auto result = std::make_unique<CollapsibleTreeItem>(mpRoot, this, "", baseElement.GetName());
//   
//   for (auto it = baseElement.BeginChildren(); it != baseElement.EndChildren(); it++) {
//      result->AddSubElement(ParseUIElementTitles(*it));
//   }
//   
//   return result;
//}

void ConstrainerVC::Start()
{
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

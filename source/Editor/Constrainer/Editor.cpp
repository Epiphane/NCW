// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <Engine/Logger/Logger.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Engine/UI/UIContextMenu.h>
#include <Engine/UI/UISerializationHelper.h>

#if !CUBEWORLD_PLATFORM_WINDOWS
#include <libfswatch/c++/monitor_factory.hpp>
#endif

#include "Editor.h"
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

Editor::Editor(Engine::Input* input, const Controls::Options& options) : UIRoot(input)
{
   Sidebar* sidebar = Add<Sidebar>();
   Controls* controls = Add<Controls>(options);

   sidebar->ConstrainLeftAlignedTo(this);
   sidebar->ConstrainTopAlignedTo(this);
   sidebar->ConstrainWidthTo(this, 0.0, 0.2);
   sidebar->ConstrainAbove(controls);

   controls->ConstrainLeftAlignedTo(this);
   controls->ConstrainBottomAlignedTo(this);
   controls->ConstrainWidthTo(sidebar);
   
   
   std::vector<std::string> farts = {"lol.txt"};
#if !CUBEWORLD_PLATFORM_WINDOWS
   //  fsw::monitor* boyo = fsw::monitor_factory::create_monitor(system_default_monitor_type, farts, nullptr);
#endif

//   TextButton::Options buttonOptions;
//   buttonOptions.text = "I'm a big boy";
//   buttonOptions.onClick = std::bind(&Editor::BigDumbTest, this);
//   mTestContextMenuButton = Add<TextButton>(buttonOptions);
//
//   mTestContextMenuButton->ConstrainWidth(100);
//   mTestContextMenuButton->ConstrainHeight(30);
//   mTestContextMenuButton->ConstrainCenterTo(this);

   Engine::UISerializationHelper serializer;

   Engine::ElementsByName elementMap = serializer.CreateUIFromJSONFile(Paths::Normalize(Asset::UIElement("example_ui_serialized.json")), mpRoot, mContentLayer);

   UIElement* mainContent = elementMap["TestJSONStuff"];
   mainContent->ConstrainHeightTo(this);
   mainContent->ConstrainToRightOf(sidebar);
   mainContent->ConstrainTopAlignedTo(this);
   mainContent->ConstrainRightAlignedTo(this);

   UIElement* wrapperLayer = Add<UIElement>();
   wrapperLayer->ConstrainInFrontOfAllDescendants(mainContent);

   for (auto elementPair : elementMap) {
      UIElement* elementToWrap = elementPair.second;
      std::string wrapperName = elementToWrap->GetName();
      wrapperName.append("_DesignWrapper");

      wrapperLayer->Add<UIElementDesignerWrapper>(wrapperName, elementToWrap);
   }
//   std::list<UIContextMenu::Choice> bleh = {
//         {"Test this out", std::bind(&Editor::TestButton, this)}
//   };

//   UIContextMenu *testMenu = mpRoot->Add<UIContextMenu>("TestThingy", bleh);
//   testMenu->ConstrainCenterTo(this);
//   testMenu->ConstrainInFrontOfAllDescendants(mainContent);
}

void Editor::BigDumbTest()
{
   std::list<UIContextMenu::Choice> bleh = {
         {"Test this out", std::bind(&Editor::TestButton, this)},
         {"Test THIS out", std::bind(&Editor::TestButton, this)},
         {"Test THIS out", std::bind(&Editor::TestButton, this)}
   };

   mpRoot->CreateUIContextMenu(200, 200, bleh);
}

void Editor::TestButton() {
   LogDebugInfo();
}

void Editor::Start()
{
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

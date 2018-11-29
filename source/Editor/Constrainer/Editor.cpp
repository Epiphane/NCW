// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Engine/UI/UIContextMenu.h>
#include <Engine/UI/UISerializationHelper.h>

#include "Editor.h"
#include "Sidebar.h"

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

   TextButton::Options buttonOptions;
   buttonOptions.text = "I'm a big boy";
   buttonOptions.onClick = std::bind(&Editor::BigDumbTest, this);
   TextButton* bigBoyButton = Add<TextButton>(buttonOptions);

   bigBoyButton->ConstrainWidth(100);
   bigBoyButton->ConstrainHeight(30);
   bigBoyButton->ConstrainCenterTo(this);

//   Engine::UISerializationHelper serializer;
//
//   Engine::ElementsByName elementMap = serializer.CreateUIFromJSONFile(Paths::Normalize(Asset::UIElement("example_ui_serialized.json")), mpRoot, this);
//
//   UIElement* mainContent = elementMap["TestJSONStuff"];
//   mainContent->ConstrainHeightTo(this);
//   mainContent->ConstrainToRightOf(sidebar);
//   mainContent->ConstrainTopAlignedTo(this);
//   mainContent->ConstrainRightAlignedTo(this);

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

   mpRoot->CreateUIContextMenu(100, 200, bleh);
}

void Editor::TestButton() {
}

void Editor::Start()
{
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

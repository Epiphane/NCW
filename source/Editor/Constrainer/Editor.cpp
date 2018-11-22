// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Engine/UI/UISerializationHelper.h>

#include "Editor.h"
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

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

   Engine::UISerializationHelper serializer;

   UIElement* mainContent = serializer.CreateUIFromJSONFile(Paths::Normalize(Asset::UIElement("example_ui_serialized.json")), mpRoot, this);
   mainContent->ConstrainHeightTo(this);
   mainContent->ConstrainToRightOf(sidebar);
   mainContent->ConstrainTopAlignedTo(this);
   mainContent->ConstrainRightAlignedTo(this);
}

void Editor::Start()
{
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

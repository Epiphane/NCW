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

Editor::Editor()
{
   Sidebar* sidebar = Add<Sidebar>();

   sidebar->ConstrainLeftAlignedTo(this);
   sidebar->ConstrainTopAlignedTo(this);
   sidebar->ConstrainWidthTo(this, 0.0, 0.2);
   sidebar->ConstrainHeightTo(this);

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

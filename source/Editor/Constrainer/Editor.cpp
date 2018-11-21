// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <Shared/DebugHelper.h>

//#include "Dock.h"
#include "Editor.h"
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

Editor::Editor(const Controls::Options& options)
{
   Sidebar* sidebar = Add<Sidebar>();
   Controls* controls = Add<Controls>(options);
   
   sidebar->ConstrainLeftAlignedTo(this);
   sidebar->ConstrainTopAlignedTo(this);
   sidebar->ConstrainWidthTo(this, 0.0, 0.2);
   sidebar->ConstrainHeightTo(this);

   controls->ConstrainLeftAlignedTo(this);
   controls->ConstrainBottomAlignedTo(this);
   controls->ConstrainWidthTo(sidebar);
}

void Editor::Start()
{
//   DebugHelper::Instance()->SetBounds(&mStateWindow->GetFrame());
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

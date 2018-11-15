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

Editor::Editor(Bounded& parent)
    : UIRoot(parent)
{
   Sidebar* sidebar = Add<Sidebar>();
   
   sidebar->ConstrainLeftAlignedTo(this);
   sidebar->ConstrainTopAlignedTo(this);
   sidebar->ConstrainHeightTo(this);
   sidebar->ConstrainWidthTo(this, 0.0, 0.2);
//   Dock* dock = Add<Dock>();

   // Organize everything
//   Engine::UIFrame& fSidebar = sidebar->GetFrame();
//   Engine::UIFrame& fDock = dock->GetFrame();
//   mSolver.add_constraints({
//
//      fDock.left == mFrame.left + fSidebar.width,
//      fDock.bottom == mFrame.bottom,
//      fDock.right == mFrame.right,
//      fDock.height == mFrame.height * 0.4,
//   });
//
//   mStateWindow->SetState(std::move(state));
}

void Editor::Start()
{
//   DebugHelper::Instance()->SetBounds(&mStateWindow->GetFrame());
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

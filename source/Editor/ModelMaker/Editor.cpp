// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <Shared/DebugHelper.h>

#include "Dock.h"
#include "Editor.h"
#include "Sidebar.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

using UI::StateWindow;

Editor::Editor(Bounded& parent) : UIRoot(parent)
{
   // I wanna do this better
   mStateWindow = Add<StateWindow>(nullptr);
   std::unique_ptr<MainState> state{new MainState(Engine::Window::Instance(), mStateWindow->GetFrame())};
   state->SetParent(this);

   Sidebar* sidebar = Add<Sidebar>();

   // Organize everything
   Engine::UIFrame& fSidebar = sidebar->GetFrame();
   Engine::UIFrame& fPreview = mStateWindow->GetFrame();
   mSolver.add_constraints({
      fSidebar.left == mFrame.left,
      fSidebar.top == mFrame.top,
      fSidebar.width == mFrame.width * 0.2,
      fSidebar.height == mFrame.height,

      fPreview.left == fSidebar.right,
      fPreview.top == mFrame.top,
      fPreview.right == mFrame.right,
      fPreview.bottom == mFrame.bottom,
   });

   mStateWindow->SetState(std::move(state));
}

void Editor::Start()
{
   DebugHelper::Instance()->SetBounds(&mStateWindow->GetFrame());
}

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld

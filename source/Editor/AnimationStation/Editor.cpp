// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <Engine/UI/UIRectFilled.h>
#include <Shared/DebugHelper.h>

#include "../UI/Controls.h"
#include "Dock.h"
#include "Editor.h"
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

Editor::Editor(Bounded& parent) : UIRoot(parent)
{
   // I wanna do this better
   mStateWindow = Add<StateWindow>(nullptr);
   std::unique_ptr<MainState> state{new MainState(Engine::Window::Instance(), mStateWindow->GetFrame())};
   state->SetParent(this);
   mStateWindow->SetState(std::move(state));

   Sidebar* sidebar = Add<Sidebar>();
   Dock* dock = Add<Dock>();

   // Organize everything
   Engine::UIFrame& fSidebar = sidebar->GetFrame();
   Engine::UIFrame& fDock = dock->GetFrame();
   Engine::UIFrame& fPreview = mStateWindow->GetFrame();
   mSolver.add_constraints({
      fSidebar.left == mFrame.left,
      fSidebar.top == mFrame.top,
      fSidebar.width == mFrame.width * 0.2,
      fSidebar.height == mFrame.height * 0.8,

      fDock.left == mFrame.left + fSidebar.width,
      fDock.bottom == mFrame.bottom,
      fDock.right == mFrame.right,
      fDock.height == mFrame.height * 0.4,

      fPreview.left == fSidebar.right,
      fPreview.top == mFrame.top,
      fPreview.right == mFrame.right,
      fPreview.bottom == fDock.top,
   });
}

void Editor::Start()
{
   Game::DebugHelper::Instance()->SetBounds(&mStateWindow->GetFrame());
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

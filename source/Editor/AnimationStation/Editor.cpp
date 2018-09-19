// By Thomas Steinke

#include <Engine/Core/StateManager.h>
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

Editor::Editor(
   Bounded& parent,
   const Options& options
)
   : SubWindow(parent, options)
{
   // Sidebar options
   Sidebar::Options controlsOptions;
   controlsOptions.y = 0.2f;
   controlsOptions.w = 0.2f;
   controlsOptions.h = 0.8f;

   // Dock options
   Dock::Options dockOptions;
   dockOptions.x = controlsOptions.w;
   dockOptions.w = 1.0f - controlsOptions.w;
   dockOptions.h = 0.4f;

   // Preview of skeleton in the game state
   StateWindow::Options currentEditorOptions;
   currentEditorOptions.x = controlsOptions.w;
   currentEditorOptions.y = dockOptions.h;
   currentEditorOptions.w = 1.0f - controlsOptions.w;
   currentEditorOptions.h = 1.0f - dockOptions.h;

   mStateWindow = Add<StateWindow>(currentEditorOptions);
   mState = std::make_unique<MainState>(Engine::Window::Instance(), *mStateWindow);
   mState->Load();

   mSidebar = Add<Sidebar>(controlsOptions, mState.get());
   mDock = Add<Dock>(dockOptions, mState.get());
}

void Editor::Start()
{
   Engine::StateManager* stateManager = Engine::StateManager::Instance();
   stateManager->SetState(mState.get());

   Game::DebugHelper::Instance()->SetBounds(mStateWindow);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

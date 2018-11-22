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

Editor::Editor(Engine::Input* input, const Controls::Options& options) : UIRoot(input)
{
   // I wanna do this better
   mStateWindow = Add<StateWindow>(nullptr);
   std::unique_ptr<MainState> state{new MainState(mStateWindow, mStateWindow->GetFrame())};
   state->SetParent(this);

   Sidebar* sidebar = Add<Sidebar>();
   Controls* controls = Add<Controls>(options);

   // Organize everything
   sidebar->ConstrainLeftAlignedTo(this);
   sidebar->ConstrainTopAlignedTo(this);
   sidebar->ConstrainWidthTo(this, 0, 0.2);
   sidebar->ConstrainAbove(controls);

   controls->ConstrainLeftAlignedTo(this);
   controls->ConstrainBottomAlignedTo(this);
   controls->ConstrainWidthTo(sidebar);

   mStateWindow->ConstrainToRightOf(sidebar);
   mStateWindow->ConstrainHeightTo(this);
   mStateWindow->ConstrainTopAlignedTo(this);
   mStateWindow->ConstrainRightAlignedTo(this);

   mStateWindow->SetState(std::move(state));
}

void Editor::Start()
{
   DebugHelper::Instance()->SetBounds(&mStateWindow->GetFrame());
}

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld

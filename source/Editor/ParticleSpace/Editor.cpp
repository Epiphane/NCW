// By Thomas Steinke

#include <Engine/Core/StateManager.h>
#include <Shared/DebugHelper.h>

#include "Dock.h"
#include "Editor.h"
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

using UI::StateWindow;

Editor::Editor(Engine::Input* input, const Controls::Options& options) : UIRoot(input)
{
   // I wanna do this better
   mStateWindow = Add<StateWindow>(nullptr);
   std::unique_ptr<MainState> state{new MainState(mStateWindow, mStateWindow->GetFrame())};
   state->SetParent(this);
   state->TransformParentEvents<MouseDownEvent>(mStateWindow);
   state->TransformParentEvents<MouseUpEvent>(mStateWindow);
   state->TransformParentEvents<MouseClickEvent>(mStateWindow);

   Sidebar* sidebar = Add<Sidebar>();
   Controls* controls = Add<Controls>(options);
   Dock* dock = Add<Dock>();

   // Organize everything
   sidebar->ConstrainLeftAlignedTo(this);
   sidebar->ConstrainTopAlignedTo(this);
   sidebar->ConstrainWidthTo(this, 0, 0.2);
   sidebar->ConstrainAbove(controls);

   controls->ConstrainLeftAlignedTo(this);
   controls->ConstrainBottomAlignedTo(this);
   controls->ConstrainWidthTo(sidebar);

   dock->ConstrainToRightOf(sidebar);
   dock->ConstrainRightAlignedTo(this);
   dock->ConstrainBottomAlignedTo(this);
   dock->ConstrainHeightTo(this, 0, 0.4);

   mStateWindow->ConstrainLeftAlignedTo(dock);
   mStateWindow->ConstrainTopAlignedTo(this);
   mStateWindow->ConstrainAbove(dock);
   mStateWindow->ConstrainRightAlignedTo(dock);

   mStateWindow->SetState(std::move(state));
}

void Editor::Start()
{
   DebugHelper::Instance().SetBounds(&mStateWindow->GetFrame());
}

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld

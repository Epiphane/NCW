// By Thomas Steinke

#include <imgui.h>
#include <Engine/Core/StateManager.h>
#include <Shared/DebugHelper.h>

#include "Dock.h"
#include "Editor.h"
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

Editor::Editor(Engine::Input* input, const Controls::Options& options) : UIRoot(input), mInput(*input)
{
   // I wanna do this better
   mStateWindow.reset(new StateWindow(*input, 600, 400, *mpRoot->GetAggregator<Aggregator::Image>(), nullptr));
   std::unique_ptr<MainState> state{new MainState(mStateWindow.get(), *mStateWindow)};
   state->SetParent(this);
   state->TransformParentEvents<MouseDownEvent>(mStateWindow.get());
   state->TransformParentEvents<MouseUpEvent>(mStateWindow.get());
   state->TransformParentEvents<MouseClickEvent>(mStateWindow.get());

   Sidebar* sidebar = Add<Sidebar>();
   Controls* controls = Add<Controls>(options);
   // Dock* dock = Add<Dock>();

   // Organize everything
   sidebar->ConstrainLeftAlignedTo(this);
   sidebar->ConstrainTopAlignedTo(this);
   sidebar->ConstrainWidthTo(this, 0, 0.2);
   sidebar->ConstrainAbove(controls);

   controls->ConstrainLeftAlignedTo(this);
   controls->ConstrainBottomAlignedTo(this);
   controls->ConstrainWidthTo(sidebar);

   // dock->ConstrainToRightOf(sidebar);
   // dock->ConstrainRightAlignedTo(this);
   // dock->ConstrainBottomAlignedTo(this);
   // dock->ConstrainHeightTo(this, 0, 0.4);

   // mStateWindow->ConstrainLeftAlignedTo(dock);
   // mStateWindow->ConstrainTopAlignedTo(this);
   // mStateWindow->ConstrainAbove(dock);
   // mStateWindow->ConstrainRightAlignedTo(dock);

   mStateWindow->SetState(std::move(state));
}

void Editor::Start()
{
   DebugHelper::Instance().SetBounds(mStateWindow.get());
}

void Editor::Update(TIMEDELTA dt)
{
   UIRoot::Update(dt);

   ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_Always);
   ImGui::Begin("Whatever");
   ImGui::Text("dear imgui says hello. (%s)", IMGUI_VERSION);
   ImGui::Spacing();
   ImGui::End();

   ImGui::Begin("Whatever2");
   ImGui::Text("dear imgui says hello. (%s)", IMGUI_VERSION);
   ImGui::Spacing();
   // Get position of game window
   ImVec2 cursor = ImGui::GetCursorScreenPos();
   glm::tvec2<double> corrected = mStateWindow->CorrectYCoordinate({(double)cursor.x, (double)cursor.y});
   mStateWindow->SetPosition((uint32_t)corrected.x, (uint32_t)corrected.y - mStateWindow->GetHeight());
   ImGui::ImageButton(
      (ImTextureID)(intptr_t)mStateWindow->GetFramebuffer().GetTexture(),
      ImVec2(mStateWindow->GetWidth(), mStateWindow->GetHeight()),
      ImVec2(0, 1),
      ImVec2(1, 0),
      0
   );
   ImGui::End();

   mStateWindow->Update(dt);
}

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

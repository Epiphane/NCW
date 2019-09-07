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

namespace AnimationStation
{

Editor::Editor(Engine::Input& input)
   : UIRoot(&input)
{
   // I wanna do this better
   mStateWindow.reset(new StateWindow(input, 600, 400, *mpRoot->GetAggregator<Aggregator::Image>(), nullptr));
   std::unique_ptr<MainState> state{new MainState(mStateWindow.get(), *mStateWindow)};
   state->SetParent(this);
   state->TransformParentEvents<MouseDownEvent>(mStateWindow.get());
   state->TransformParentEvents<MouseUpEvent>(mStateWindow.get());
   state->TransformParentEvents<MouseClickEvent>(mStateWindow.get());

   Add<Sidebar>();
   Add<Dock>();

   mStateWindow->SetState(std::move(state));
}

void Editor::Start()
{
   DebugHelper::Instance().SetBounds(nullptr);
}

void Editor::Update(TIMEDELTA dt)
{
   UIRoot::Update(dt);

   ImGui::SetNextWindowPos(ImVec2(250, 20), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(1000, 500), ImGuiCond_FirstUseEver);
   ImGui::Begin("Preview");
   ImVec2 space = ImGui::GetContentRegionAvail();
   if (space.y > 0)
   {
      ImVec2 pos = ImGui::GetCursorScreenPos();
      glm::tvec2<double> corrected = mStateWindow->CorrectYCoordinate({
         (double)pos.x,
         (double)pos.y + mStateWindow->GetHeight()
      });
      mStateWindow->SetPosition(corrected);
      mStateWindow->SetSize(space);
      ImGui::ImageButton(
         (ImTextureID)(intptr_t)mStateWindow->GetFramebuffer().GetTexture(),
         ImVec2((float)mStateWindow->GetWidth(), (float)mStateWindow->GetHeight()),
         ImVec2(0, 1),
         ImVec2(1, 0),
         0
      );
   }
   ImGui::End();

   mStateWindow->Update(dt);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

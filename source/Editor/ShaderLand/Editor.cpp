// By Thomas Steinke

#include <imgui.h>
#include <Engine/Core/StateManager.h>
#include <Shared/DebugHelper.h>

#include "Editor.h"
#include "Mesh.h"
#include "State.h"
#include "Tools.h"

namespace CubeWorld
{

namespace Editor
{

namespace ShaderLand
{

Editor::Editor(Engine::Input& input)
    : UIRoot(&input)
{
    // Add tools first, so that it can get a handle on the mesh once that's added below
    Add<Tools>();
    Add<Mesh>();

    // I wanna do this better
    mStateWindow.reset(new StateWindow(input, 600, 400, nullptr));
    std::unique_ptr<MainState> state{ new MainState(mStateWindow.get(), *mStateWindow) };
    state->SetParent(this);
    state->TransformParentEvents<MouseDownEvent>(mStateWindow.get());
    state->TransformParentEvents<MouseUpEvent>(mStateWindow.get());
    state->TransformParentEvents<MouseClickEvent>(mStateWindow.get());
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
    ImGui::SetNextWindowSize(ImVec2(740, 700), ImGuiCond_FirstUseEver);
    ImGui::Begin("Shader Land");
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

}; // namespace ShaderLand

}; // namespace Editor

}; // namespace CubeWorld

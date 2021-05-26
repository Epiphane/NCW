// By Thomas Steinke

#include <imgui.h>

#include <RGBFileSystem/File.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/JSONSerializer.h>
#include <RGBSettings/SettingsProvider.h>
#include <RGBText/Encoding.h>
#include <Engine/Core/Window.h>
#include <Shared/Helpers/Asset.h>

#include <Shared/Imgui/Extensions.h>
#include <Shared/Imgui/Meta.h>
#include "Mesh.h"

namespace CubeWorld
{

namespace Editor
{

namespace ShaderLand
{

Mesh::Mesh(Engine::UIRoot* root, UIElement* parent)
    : UIElement(root, parent, "ShaderLandMesh")
{
    mVertices = {
        { 1, 0, 0, 1 },
        { 0, 1, 0, 1 },
        { 0, 0, 1, 1 },
    };

    mColors = {
        { 1, 0, 0, 1 },
        { 0, 1, 0, 1 },
        { 0, 0, 1, 1 },
    };

    mNormals = {
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 },
    };

    mIndices = { 0, 1, 2, -1 };

    mFilename = Asset::Path(Paths::Join("Custom", "ShaderLand.Mesh.json"));
    LoadFile(mFilename);

    root->Subscribe<Engine::ComponentAddedEvent<ShadedMesh>>(*this);
    root->Subscribe<Engine::ComponentRemovedEvent<ShadedMesh>>(*this);
}

void Mesh::Update(TIMEDELTA)
{
    ImGui::Begin("Mesh");
    Imgui::Draw("Vertices", mVertices);
    Imgui::Draw("Colors", mColors);
    Imgui::Draw("Normals", mNormals);

    // Do indices separately just for fun
    if (ImGui::TreeNode("Indices"))
    {
        size_t index = 0;
        for (int32_t& item : mIndices)
        {
            Imgui::Draw(FormatString("##index{num}", index), item);
            index++;
        }
        if (ImGui::Button("4 more"))
        {
            mIndices.resize(mIndices.size() + 4, -1);
        }
        ImGui::SameLine();
        if (ImGui::Button("4 less"))
        {
            mIndices.resize(mIndices.size() - 4);
        }
        ImGui::TreePop();
    }

    const char* selected = "unknown";
    switch (mRenderType)
    {
    case GL_TRIANGLES:
        selected = "GL_TRIANGLES";
        break;
    case GL_TRIANGLE_FAN:
        selected = "GL_TRIANGLE_FAN";
        break;
    case GL_TRIANGLE_STRIP:
        selected = "GL_TRIANGLE_STRIP";
        break;
    }
    if (ImGui::BeginCombo("Render Type", selected))
    {
        if (ImGui::Selectable("GL_TRIANGLES", mRenderType == GL_TRIANGLES))
        {
            mRenderType = GL_TRIANGLES;
        }
        if (ImGui::Selectable("GL_TRIANGLE_FAN", mRenderType == GL_TRIANGLE_FAN))
        {
            mRenderType = GL_TRIANGLE_FAN;
        }
        if (ImGui::Selectable("GL_TRIANGLE_STRIP", mRenderType == GL_TRIANGLE_STRIP))
        {
            mRenderType = GL_TRIANGLE_STRIP;
        }

        ImGui::EndCombo();
    }

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_Always);
    ImGui::Begin("File", nullptr, ImGuiWindowFlags_NoResize);

    ImVec2 space = ImGui::GetContentRegionAvail();
    if (ImGui::Button("Open", ImVec2(space.x, 0)))
    {
        LoadNewFile();
    }

    float halfSize = (space.x - ImGui::GetStyle().ItemSpacing.x) / 2;
    if (ImGui::Button("Save", ImVec2(halfSize, 0)))
    {
        SaveFile();
    }

    ImGui::SameLine();
    if (ImGui::Button("Save as", ImVec2(halfSize, 0)))
    {
        SaveNewFile();
    }

    ImGui::End();

    UpdateMesh();
}

void Mesh::LoadNewFile()
{
    std::string file = OpenFileDialog(mFilename, {});
    if (!file.empty())
    {
        mFilename = file;
        SettingsProvider::Instance().Set("shader_land", "filename", file);
        LoadFile(file);
    }
}

void Mesh::SaveNewFile()
{
    std::string file = SaveFileDialog(mFilename);
    if (!file.empty())
    {
        mFilename = file;
        mSaveData = BindingProperty{};
        SaveFile();
    }
}

BindingProperty Mesh::Serialize()
{
    BindingProperty serialized;

    std::for_each(mVertices.begin(), mVertices.end(), [&](const glm::vec4& v) {
        serialized["vertices"].push_back(v);
    });

    std::for_each(mColors.begin(), mColors.end(), [&](const glm::vec4& v) {
        serialized["colors"].push_back(v);
    });

    std::for_each(mNormals.begin(), mNormals.end(), [&](const glm::vec4& v) {
        serialized["normals"].push_back(v);
    });

    std::for_each(mIndices.begin(), mIndices.end(), [&](const int32_t& v) {
        serialized["indices"].push_back(v);
    });

    serialized["renderType"] = mRenderType;

    return serialized;
}

void Mesh::SaveFile()
{
    BindingProperty state = Serialize();
    if (state != mSaveData)
    {
        mSaveData = state;

        if (auto result = DiskFileSystem{}.MakeDirectory(Paths::GetDirectory(mFilename)); !result)
        {
            result.Failure().WithContext("Failed creating directory {}", Paths::GetDirectory(mFilename)).Log();
        }

        if (auto result = JSONSerializer::SerializeFile(mFilename, mSaveData); !result)
        {
            result.Failure().WithContext("Failed saving {}", mFilename).Log();
        }
    }
}

void Mesh::LoadFile(const std::string& filename)
{
    Maybe<BindingProperty> maybeState = JSONSerializer::DeserializeFile(filename);
    if (!maybeState)
    {
        return;
    }

    mSaveData = std::move(*maybeState);
    mVertices.clear();
    mColors.clear();
    mNormals.clear();
    mIndices.clear();

    for (const auto& vertex : mSaveData["vertices"])
    {
        mVertices.push_back(vertex.GetVec4());
    }

    for (const auto& color : mSaveData["colors"])
    {
        mColors.push_back(color.GetVec4());
    }

    for (const auto& normal : mSaveData["normals"])
    {
        mNormals.push_back(normal.GetVec4());
    }

    for (const auto& index : mSaveData["indices"])
    {
        mIndices.push_back(index.GetIntValue());
    }

    mRenderType = mSaveData["renderType"].GetUintValue(GL_TRIANGLES);
}

void Mesh::UpdateMesh()
{
    if (!mMesh)
    {
        return;
    }

    if (mIndices.size() % 4 != 0)
    {
        mIndices.resize(size_t(std::ceil(mIndices.size() / 4) * 4), -1);
    }

    mMesh->mVertices.BufferData(mVertices);
    mMesh->mColors.BufferData(mColors);
    mMesh->mNormals.BufferData(mNormals);
    mMesh->mIndices.BufferData(mIndices);
    mMesh->mVertexCount = mVertices.size();
    mMesh->mIndexCount = mIndices.size();
    mMesh->renderType = mRenderType;

    SaveFile();
}

void Mesh::Receive(const Engine::ComponentAddedEvent<ShadedMesh>& evt)
{
    mMesh = evt.component;

    UpdateMesh();
}

void Mesh::Receive(const Engine::ComponentRemovedEvent<ShadedMesh>&)
{
    mMesh = Engine::ComponentHandle<ShadedMesh>();
}

}; // namespace ShaderLand

}; // namespace Editor

}; // namespace CubeWorld

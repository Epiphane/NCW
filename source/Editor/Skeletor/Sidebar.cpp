// By Thomas Steinke

#include <stack>
#include <imgui.h>
#include <RGBDesignPatterns/Macros.h>
#include <RGBFileSystem/File.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <RGBSettings/SettingsProvider.h>
#include <Engine/Core/Window.h>
#include <Shared/Helpers/Asset.h>

#include "../Imgui/Extensions.h"
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

Sidebar::Sidebar(Engine::UIRoot* root, Engine::UIElement* parent)
   : UIElement(root, parent, "SkeletorSidebar")
   , mModified(false)
{
   mFilename = SettingsProvider::Instance().Get("animation_station", "filename").GetStringValue();
   if (mFilename.empty())
   {
      mFilename = Asset::Skeleton("greatmace.yaml");
   }

   root->Subscribe<Engine::ComponentAddedEvent<SkeletonCollection>>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<Skeleton>>(*this);
   root->Subscribe<SkeletonModifiedEvent>(*this);

   LoadFile(mFilename);
}

void Sidebar::Update(TIMEDELTA)
{
   ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_Always);
   ImGui::Begin("File", nullptr, ImGuiWindowFlags_NoResize);

   ImVec2 space = ImGui::GetContentRegionAvail();
   if (ImGui::Button("Open", ImVec2(space.x, 0)))
   {
      LoadNewFile();
   }

   float halfSize = (space.x - ImGui::GetStyle().ItemSpacing.x) / 2;
   if (ImGuiEx::Button(mModified, "Save", ImVec2(halfSize, 0)))
   {
      SaveFile();
   }

   ImGui::SameLine();
   if (ImGui::Button("Save as", ImVec2(halfSize, 0)))
   {
      SaveNewFile();
   }

   ImGui::End();
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<SkeletonCollection>&)
{
   LoadFile(mFilename);
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<Skeleton>& evt)
{
   // Lol beautiful trickery: we add skeleton parts from the root-most first,
   // up to the final piece being the one the user wants to load. Therefore,
   // by the time we hit any other business logic this will be the right one.
   mSkeleton = evt.component;
}

void Sidebar::Receive(const SkeletonModifiedEvent&)
{
   SetModified(true);
}

void Sidebar::SetModified(bool modified)
{
   if (mModified == modified)
   {
      // Unchanged.
      return;
   }

   mModified = modified;

   std::string title = "NCW - Skeletor - ";
   if (mModified)
   {
      title += "*";
   }
   title += Paths::GetFilename(mFilename);
   Engine::Window::Instance().SetTitle(title);
}

void Sidebar::LoadNewFile()
{
   std::string file = OpenFileDialog(mFilename, {});
   if (!file.empty())
   {
      mFilename = file;
      SettingsProvider::Instance().Set("skeletor", "filename", file);
      LoadFile(file);
   }
}

void Sidebar::LoadFile(const std::string& filename)
{
   mpRoot->Emit<SkeletonClearedEvent>();

   std::stack<std::string> parts;
   std::string currentFile = filename;
   do
   {
      Maybe<BindingProperty> maybeData = YAMLSerializer::DeserializeFile(currentFile);
      if (!maybeData)
      {
         maybeData.Failure().WithContext("Failed to deserialize file {path}", currentFile).Log();
         break;
      }
      const BindingProperty data = std::move(*maybeData);

      parts.push(currentFile);

      std::string parent = data["parent"];
      if (parent == "")
      {
         currentFile = "";
      }
      else
      {
         currentFile = Paths::Join(Paths::GetDirectory(currentFile), parent + ".yaml");
      }
   }
   while (currentFile != "");

   while (!parts.empty())
   {
      mpRoot->Emit<AddSkeletonPartEvent>(parts.top());
      parts.pop();
   }

   mpRoot->Emit<SkeletonLoadedEvent>(mSkeleton);
   SetModified(false);
}

void Sidebar::SaveNewFile()
{
   std::string file = SaveFileDialog(mFilename);
   if (!file.empty())
   {
      mFilename = file;
      SaveFile();
   }
}

void Sidebar::SaveFile()
{
   mpRoot->Emit<SuspendEditingEvent>();
   BindingProperty serialized = mSkeleton->Serialize();
   mpRoot->Emit<ResumeEditingEvent>();
   Maybe<void> written = YAMLSerializer{}.SerializeFile(mFilename, serialized);
   if (!written)
   {
      written.Failure().WithContext("Failed writing file {path}", mFilename).Log();
   }

   mpRoot->Emit<SkeletonSavedEvent>(mSkeleton);
   SetModified(false);
}

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

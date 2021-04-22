// By Thomas Steinke

#include <stack>
#include <imgui.h>
#include <RGBFileSystem/File.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <RGBSettings/SettingsProvider.h>
#include <Engine/Core/Window.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/Helpers/Asset.h>

#include <Shared/Imgui/Extensions.h>
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

Sidebar::Sidebar(Engine::UIRoot* root, Engine::UIElement* parent)
    : UIElement(root, parent, "AnimationStationSidebar")
    , mModified(true)
{
   root->Subscribe<Engine::ComponentAddedEvent<SimpleAnimationController>>(*this);
   root->Subscribe<SkeletonModifiedEvent>(*this);

   mFilename = SettingsProvider::Instance().Get("animation_station", "filename").GetStringValue();
   if (mFilename.empty())
   {
      mFilename = Asset::Skeleton("greatmace.yaml");
   }
}

void Sidebar::Update(TIMEDELTA)
{
   ImGui::SetNextWindowPos(ImVec2(25, 20), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_Always);
   ImGui::Begin("File", nullptr, ImGuiWindowFlags_NoResize);

   ImVec2 space = ImGui::GetContentRegionAvail();
   if (ImGui::Button("Open", ImVec2(space.x, 0)))
   {
      LoadNewFile();
   }

   if (ImGuiEx::Button(mModified, "Save", ImVec2(space.x, 0)))
   {
      SaveFile();
   }

   if (ImGui::Button("Discard Changes", ImVec2(space.x, 0)))
   {
      DiscardChanges();
   }

   ImGui::End();
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<SimpleAnimationController>& evt)
{
   mSkeleton = evt.component;

   LoadFile(mFilename);
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

   std::string title = "NCW - Animation Station - ";
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
      SettingsProvider::Instance().Set("animation_station", "filename", file);
      LoadFile(file);
   }
}

void Sidebar::LoadFile(const std::string& filename)
{
   mpRoot->Emit<SuspendEditingEvent>();
   mpRoot->Emit<SkeletonClearedEvent>();

   std::stack<std::string> parts;
   std::string currentFile = filename;
   do
   {
      std::string name = Paths::GetFilename(currentFile);

      Maybe<BindingProperty> maybeData = YAMLSerializer::DeserializeFile(currentFile);
      if (!maybeData)
      {
         maybeData.Failure().WithContext("Failed to deserialize {path}", currentFile).Log();
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
         currentFile = Paths::Join(Paths::GetDirectory(filename), parent) + ".yaml";
      }
   } while (currentFile != "");

   while (!parts.empty())
   {
      mpRoot->Emit<AddSkeletonPartEvent>(parts.top());
      parts.pop();
   }

   mpRoot->Emit<SkeletonLoadedEvent>(mSkeleton);
   SetModified(false);
}

void Sidebar::SaveFile()
{
   mpRoot->Emit<SuspendEditingEvent>();
   mSkeleton->UpdateSkeletonStates();

   for (const Engine::ComponentHandle<SkeletonAnimations>& anims : mSkeleton->animations)
   {
      BindingProperty serialized = anims->Serialize();

      for (const auto& animation : serialized["states"])
      {
         std::string path = Asset::Animation(Paths::Join(anims->entity, animation["name"].GetStringValue() + ".yaml"));
         Maybe<void> result = YAMLSerializer::SerializeFile(path, animation);
         if (!result)
         {
            result.Failure().WithContext("Failed saving file {path}", path).Log();
         }
      }
   }

   mpRoot->Emit<SkeletonSavedEvent>(mSkeleton);
   mpRoot->Emit<ResumeEditingEvent>();
   SetModified(false);
}

void Sidebar::DiscardChanges()
{
   LoadFile(mFilename);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

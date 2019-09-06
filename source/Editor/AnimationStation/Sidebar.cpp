// By Thomas Steinke

#include <imgui.h>
#include <RGBFileSystem/File.h>
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

namespace AnimationStation
{

using Engine::UIRoot;
using UI::RectFilled;
using UI::TextButton;

Sidebar::Sidebar(UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "AnimationStationSidebar", glm::vec4(0.2, 0.2, 0.2, 1))
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
   ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
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

   if (mModified)
   {
      if (ImGui::Button("Discard Changes", ImVec2(space.x, 0)))
      {
         DiscardChanges();
      }
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
         LOG_ERROR("Failed to deserialize file %1: %2", currentFile, maybeData.Failure().GetMessage());
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

      for (const auto&[name, animation] : serialized.pairs())
      {
         std::string path = Asset::Animation(Paths::Join(anims->entity, name.GetStringValue() + ".yaml"));
         Maybe<void> result = YAMLSerializer::SerializeFile(path, animation);
         if (!result)
         {
            LOG_ERROR("Failed saving file %1: %2", path, result.Failure().GetMessage());
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

// By Thomas Steinke

#include <imgui.h>

#include <RGBFileSystem/File.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <RGBSettings/SettingsProvider.h>
#include <RGBText/Encoding.h>
#include <Engine/Core/Window.h>
#include <Shared/Helpers/Asset.h>

#include <Shared/Imgui/Extensions.h>
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

Sidebar::Sidebar(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent, "ParticleSpaceSidebar")
   , mModified(true)
{
   mFilename = SettingsProvider::Instance().Get("particle_space", "filename").GetStringValue();

   if (auto[_, exists] = DiskFileSystem{}.Exists(mFilename); !exists)
   {
      mFilename = Asset::Particle("fire");
   }

   root->Subscribe<Engine::ComponentAddedEvent<ParticleEmitter>>(*this);
   root->Subscribe<ParticleEmitterReadyEvent>(*this);
   root->Subscribe<ParticleEmitterModifiedEvent>(*this);

   SetModified(false);
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

void Sidebar::Receive(const Engine::ComponentAddedEvent<ParticleEmitter>& evt)
{
   mParticleSystem = evt.component;
}

void Sidebar::Receive(const ParticleEmitterReadyEvent&)
{
   LoadFile(mFilename);
}

void Sidebar::Receive(const ParticleEmitterModifiedEvent&)
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

   std::string title = "NCW - ParticleSpace - ";
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
      SettingsProvider::Instance().Set("particle_space", "filename", file);
      LoadFile(file);
   }
}

void Sidebar::LoadFile(const std::string& filename)
{
   mpRoot->Emit<ClearParticleEmitterEvent>();
   mpRoot->Emit<LoadParticleEmitterEvent>(filename);
   mpRoot->Emit<ParticleEmitterLoadedEvent>(mParticleSystem);
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
   BindingProperty serialized = mParticleSystem->Serialize();
   Maybe<void> written = YAMLSerializer{}.SerializeFile(mFilename, serialized);
   if (!written)
   {
      written.Failure().WithContext("Failed writing file {path}", mFilename).Log();
   }

   mpRoot->Emit<ParticleEmitterSavedEvent>(mParticleSystem);
   SetModified(false);
}

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld

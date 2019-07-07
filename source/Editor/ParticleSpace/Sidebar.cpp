// By Thomas Steinke

#include <RGBFileSystem/File.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <RGBSettings/SettingsProvider.h>
#include <Engine/Core/Window.h>
#include <Engine/UI/UIStackView.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/TextButton.h>

#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

using Engine::UIRoot;
using UI::RectFilled;
using UI::TextButton;

Sidebar::Sidebar(UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "ParticleSpaceSidebar", glm::vec4(0.2, 0.2, 0.2, 1))
   , mModified(true)
{
   mFilename = SettingsProvider::Instance().Get("particle_space", "filename").GetStringValue();
   if (mFilename.empty())
   {
      mFilename = Asset::Particle("simple");
   }

   RectFilled* foreground = Add<RectFilled>("ParticleSpaceSidebarFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainCenterTo(this);
   foreground->ConstrainDimensionsTo(this, -4);

   // Labels
   Engine::UIStackView* buttons = foreground->Add<Engine::UIStackView>("ParticleSpaceSidebarStackView");
   buttons->SetOffset(8.0);

   TextButton::Options buttonOptions;
   buttonOptions.text = "Open";
   buttonOptions.onClick = std::bind(&Sidebar::LoadNewFile, this);
   TextButton* load = buttons->Add<TextButton>(buttonOptions);

   buttonOptions.text = "Save";
   buttonOptions.onClick = std::bind(&Sidebar::SaveFile, this);
   mSave = buttons->Add<TextButton>(buttonOptions);

   buttonOptions.text = "Save As...";
   buttonOptions.onClick = std::bind(&Sidebar::SaveNewFile, this);
   TextButton* saveAs = buttons->Add<TextButton>(buttonOptions);

   buttonOptions.text = "Discard Changes";
   buttonOptions.onClick = std::bind(&Sidebar::DiscardChanges, this);
   TextButton* discard = buttons->Add<TextButton>(buttonOptions);
      
   buttonOptions.text = "Quit";
   buttonOptions.size = 13; // "> Save first!"
   buttonOptions.onClick = std::bind(&Sidebar::Quit, this);
   mQuit = buttons->Add<TextButton>(buttonOptions);

   buttons->ConstrainTopAlignedTo(foreground);
   buttons->ConstrainHorizontalCenterTo(foreground);
   buttons->ConstrainWidthTo(foreground, -12);
   load->ConstrainLeftAlignedTo(buttons, 2);
   load->ConstrainWidthTo(buttons, -4);
   load->ConstrainHeight(32);
   mSave->ConstrainDimensionsTo(load);
   mSave->ConstrainLeftAlignedTo(load);
   saveAs->ConstrainDimensionsTo(mSave);
   saveAs->ConstrainLeftAlignedTo(mSave);
   discard->ConstrainDimensionsTo(saveAs);
   discard->ConstrainLeftAlignedTo(saveAs);
   mQuit->ConstrainDimensionsTo(discard);
   mQuit->ConstrainLeftAlignedTo(discard);
      
   root->Subscribe<Engine::ComponentAddedEvent<ParticleEmitter>>(*this);
   root->Subscribe<ParticleEmitterReadyEvent>(*this);
   root->Subscribe<ParticleEmitterModifiedEvent>(*this);

   SetModified(false);
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

   mSave->SetText(modified ? "*Save" : "Save");
   mQuit->SetText("Quit");
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
      LOG_ERROR("Failed writing file: %1", written.Failure().GetMessage());
   }

   mpRoot->Emit<ParticleEmitterSavedEvent>(mParticleSystem);
   SetModified(false);
}

void Sidebar::DiscardChanges()
{
   LoadFile(mFilename);
}

void Sidebar::Quit()
{
   if (!mModified)
   {
      Engine::Window::Instance().SetShouldClose(true);
   }
   else
   {
      mQuit->SetText("Save first!");
   }
}

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld

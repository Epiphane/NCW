// By Thomas Steinke

#include <RGBFileSystem/File.h>
#include <Engine/Core/Window.h>
#include <Engine/UI/UIStackView.h>
#include <Shared/Helpers/Asset.h>

#include <Shared/UI/Image.h>
#include <Shared/UI/TextButton.h>

#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::UIElement;
using UI::Image;
using UI::TextButton;
using UI::RectFilled;

Sidebar::Sidebar(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent, "ConstrainerSidebar")
   , mFilename(Paths::Normalize(Asset::Model("untitled-ui.json")))
{
   
//   RectFilled* bg = Add<RectFilled>(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
//   bg->SetName("ConstraintSidebarBackground");
   RectFilled* fg = Add<RectFilled>("ConstrainerSidebarFG", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
   fg->SetName("ConstraintSidebarForeground");
   
   fg->ConstrainEqualBounds(this, 2.0, 2.0, 2.0, 2.0);

   Engine::UIStackView* buttonParent = Add<Engine::UIStackView>("ConstrainerSidebarStackView");
   buttonParent->SetOffset(8.0);
   {
      // Labels
      TextButton::Options buttonOptions;
      buttonOptions.text = "Load";
      buttonOptions.onClick = std::bind(&Sidebar::LoadNewFile, this);
      TextButton* load = buttonParent->Add<TextButton>(buttonOptions, "ConstrainerLoadButton");

      buttonOptions.text = "Save";
      buttonOptions.onClick = std::bind(&Sidebar::SaveFile, this);
      mSave = buttonParent->Add<TextButton>(buttonOptions, "ConstrainerSaveButton");
      
      buttonOptions.text = "Save As...";
      buttonOptions.onClick = std::bind(&Sidebar::SaveNewFile, this);
      TextButton* saveAs = buttonParent->Add<TextButton>(buttonOptions, "ConstrainerSaveAsButton");
      
      buttonOptions.text = "Discard Changes";
      buttonOptions.onClick = std::bind(&Sidebar::DiscardChanges, this);
      TextButton* discard = buttonParent->Add<TextButton>(buttonOptions, "ConstrainerDiscardButton");
      
      buttonOptions.text = "Quit";
      buttonOptions.size = 13; // "> Save first!"
      buttonOptions.onClick = std::bind(&Sidebar::Quit, this);
      mQuit = buttonParent->Add<TextButton>(buttonOptions, "ConstrainerQuitButton");
      
      load->ConstrainLeftAlignedTo(this);
      load->ConstrainWidthTo(this);
      load->ConstrainHeight(32.0);
      
      mSave->ConstrainLeftAlignedTo(this);
      mSave->ConstrainWidthTo(this);
      mSave->ConstrainHeight(32.0);
      
      saveAs->ConstrainLeftAlignedTo(this);
      saveAs->ConstrainWidthTo(this);
      saveAs->ConstrainHeight(32.0);
      
      discard->ConstrainLeftAlignedTo(this);
      discard->ConstrainWidthTo(this);
      discard->ConstrainHeight(32.0);
      
      mQuit->ConstrainLeftAlignedTo(this);
      mQuit->ConstrainWidthTo(this);
      mQuit->ConstrainHeight(32.0);
      
      buttonParent->ConstrainTopAlignedTo(this, 4.0);
      buttonParent->ConstrainLeftAlignedTo(this, 8.0);
      
      buttonParent->ConstrainInFrontOf(fg);
      discard->ConstrainInFrontOf(fg);
   }
}

void Sidebar::SetModified(bool modified)
{
   if (mModified == modified)
   {
      // Unchanged.
      return;
   }

   mModified = modified;

   std::string title = "NCW - Constrainer - ";
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
      LoadFile(file);
   }
}

void Sidebar::LoadFile(const std::string& /*filename*/)
{
   // ???
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
   // TODO
   return;
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

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld

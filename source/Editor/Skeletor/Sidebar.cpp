// By Thomas Steinke

#include <fstream>
#include <RGBFileSystem/File.h>
#include <RGBNetworking/JSONSerializer.h>
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

namespace Skeletor
{

using Engine::UIRoot;
using UI::RectFilled;
using UI::TextButton;

Sidebar::Sidebar(UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "SkeletorSidebar", glm::vec4(0.2, 0.2, 0.2, 1))
   , mFilename(Asset::Skeleton("greatmace"))
   , mModified(true)
{
   RectFilled* foreground = Add<RectFilled>("SkeletorSidebarFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainCenterTo(this);
   foreground->ConstrainDimensionsTo(this, -4);

   // Labels
   Engine::UIStackView* buttons = foreground->Add<Engine::UIStackView>("ModelMakerSidebarStackView");
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
      
   root->Subscribe<Engine::ComponentAddedEvent<SkeletonCollection>>(*this);
   root->Subscribe<Engine::ComponentAddedEvent<Skeleton>>(*this);
   root->Subscribe<SkeletonModifiedEvent>(*this);

   SetModified(false);
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<SkeletonCollection>& evt)
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
   Engine::Window::Instance()->SetTitle(title);

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

void Sidebar::LoadFile(const std::string& filename)
{
   mpRoot->Emit<SkeletonClearedEvent>();

   std::stack<std::string> parts;
   std::string currentFile = filename;
   do
   {
      Maybe<BindingProperty> maybeData = JSONSerializer::DeserializeFile(currentFile + ".json");
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
         currentFile = Paths::Join(Paths::GetDirectory(currentFile), parent);
      }
   }
   while (currentFile != "");

   while (!parts.empty())
   {
      mpRoot->Emit<AddSkeletonPartEvent>(parts.top() + ".json");
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
   std::string serialized = mSkeleton->Serialize();
   std::ofstream out(mFilename);
   out << serialized << std::endl;

   mpRoot->Emit<SkeletonSavedEvent>(mSkeleton);
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
      Engine::Window::Instance()->SetShouldClose(true);
   }
   else
   {
      mQuit->SetText("Save first!");
   }
}

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld

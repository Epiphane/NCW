// By Thomas Steinke

#include <fstream>
#include <Engine/Core/File.h>
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

namespace AnimationStation
{

using Engine::UIRoot;
using UI::RectFilled;
using UI::TextButton;

Sidebar::Sidebar(UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "AnimationStationSidebar", glm::vec4(0.2, 0.2, 0.2, 1))
   , mFilename(Paths::Normalize(Asset::Model("wood-greatmace02.json")))
   , mModified(true)
{
   RectFilled* foreground = Add<RectFilled>("AnimationStationSidebarFG", glm::vec4(0, 0, 0, 1));

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
      
   root->Subscribe<Engine::ComponentAddedEvent<AnimationController>>(*this);
   root->Subscribe<SkeletonModifiedEvent>(*this);
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<AnimationController>& evt)
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
   mSkeletonFiles.clear();

   std::stack<std::string> parts;
   std::string currentFile = filename;
   do
   {
      std::string name = Paths::GetFilename(filename);
      if (mSkeletonFiles.find(name) != mSkeletonFiles.end())
      {
         LOG_ERROR("Duplicate file %1 found in skeleton. Ummmm..idk what to do", name);
      }
      mSkeletonFiles.emplace(name, filename);
      std::ifstream file(currentFile);
      nlohmann::json data;
      file >> data;

      parts.push(currentFile);

      std::string parent = data.value("parent", "");
      if (parent == "")
      {
         currentFile = "";
      }
      else
      {
         currentFile = Paths::Join(Paths::GetDirectory(filename), parent);
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
   for (size_t i = 0; i < mSkeleton->NumSkeletons(); ++i)
   {
      Engine::ComponentHandle<AnimatedSkeleton> skeleton = mSkeleton->GetSkeleton(i);
      std::string serialized = skeleton->Serialize();

      auto filenameIt = mSkeletonFiles.find(skeleton->name);
      if (filenameIt == mSkeletonFiles.end())
      {
         LOG_ERROR("Somehow the name of this skeleton changed, idk... (name %1 not found in mapping)", skeleton->name);
      }
      std::ofstream out(skeleton->name);
      //out << serialized << std::endl;
   }

   std::string serialized;// = mSkeleton->Serialize();
   std::ofstream out(mFilename);
   //out << serialized << std::endl;

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

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

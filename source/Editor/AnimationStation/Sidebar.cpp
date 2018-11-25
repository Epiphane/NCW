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
   , mFilename(Paths::Normalize(Asset::Animation("player.json")))
{
   RectFilled* foreground = Add<RectFilled>("AnimationStationSidebarFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainCenterTo(this);
   foreground->ConstrainDimensionsTo(this, -4);

   // Labels
   Engine::UIStackView* buttons = foreground->Add<Engine::UIStackView>("ModelMakerSidebarStackView");
   buttons->SetOffset(8.0);

   TextButton::Options buttonOptions;
   buttonOptions.text = "Load";
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
      
   root->Subscribe<Engine::ComponentAddedEvent<AnimatedSkeleton>>(*this);
   root->Subscribe<SkeletonModifiedEvent>(*this);
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<AnimatedSkeleton>& evt)
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
   if (!mSkeleton)
   {
      // Wait until the component exists!
      return;
   }

   mSkeleton->Load(filename);
   mSkeleton->AddModel(AnimatedSkeleton::BoneWeights{{"torso",1.0f}}, Asset::Model("body4.cub"));
   mSkeleton->AddModel(AnimatedSkeleton::BoneWeights{{"head",1.0f}}, Asset::Model("elf-head-m02.cub"));
   mSkeleton->AddModel(AnimatedSkeleton::BoneWeights{{"hair",1.0f}}, Asset::Model("elf-hair-m09.cub"));
   mSkeleton->AddModel(AnimatedSkeleton::BoneWeights{{"left_hand",1.0f}}, Asset::Model("hand2.cub"));
   mSkeleton->AddModel(AnimatedSkeleton::BoneWeights{{"right_hand",1.0f}}, Asset::Model("hand2.cub"));
   mSkeleton->AddModel(AnimatedSkeleton::BoneWeights{{"left_foot",1.0f}}, Asset::Model("foot.cub"));
   mSkeleton->AddModel(AnimatedSkeleton::BoneWeights{{"right_foot",1.0f}}, Asset::Model("foot.cub"));

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

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld

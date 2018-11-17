// By Thomas Steinke

#include <fstream>
#include <Engine/Core/File.h>
#include <Engine/Core/Window.h>
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

using Engine::UIFrame;
using Engine::UIRoot;
using UI::RectFilled;
using UI::TextButton;

Sidebar::Sidebar(UIRoot* root, UIElement* parent)
   : UIElement(root, parent, "AnimationStationSidebar")
   , mFilename(Paths::Normalize(Asset::Animation("player.json")))
{
   {
      // Backdrop
      RectFilled* bg = Add<RectFilled>(glm::vec4(0.2, 0.2, 0.2, 1));
      RectFilled* fg = Add<RectFilled>(glm::vec4(0, 0, 0, 1));

      UIFrame& fBackground = bg->GetFrame();
      UIFrame& fForeground = fg->GetFrame();
      root->AddConstraints({
         fBackground.left == mFrame.left,
         fBackground.right == mFrame.right,
         fBackground.top == mFrame.top,
         fBackground.bottom == mFrame.bottom,
         mFrame > fForeground,

         fForeground.left == fBackground.left + 2,
         fForeground.right == fBackground.right - 2,
         fForeground.top == fBackground.top - 2,
         fForeground.bottom == fBackground.bottom,
         fForeground > fBackground,
      });
   }

   {
      // Labels
      TextButton::Options buttonOptions;
      buttonOptions.text = "Load";
      buttonOptions.onClick = std::bind(&Sidebar::LoadNewFile, this);
      TextButton* load = Add<TextButton>(buttonOptions);

      buttonOptions.text = "Save";
      buttonOptions.onClick = std::bind(&Sidebar::SaveFile, this);
      mSave = Add<TextButton>(buttonOptions);
      
      buttonOptions.text = "Save As...";
      buttonOptions.onClick = std::bind(&Sidebar::SaveNewFile, this);
      TextButton* saveAs = Add<TextButton>(buttonOptions);
      
      buttonOptions.text = "Discard Changes";
      buttonOptions.onClick = std::bind(&Sidebar::DiscardChanges, this);
      TextButton* discard = Add<TextButton>(buttonOptions);
      
      buttonOptions.text = "Quit";
      buttonOptions.size = 13; // "> Save first!"
      buttonOptions.onClick = std::bind(&Sidebar::Quit, this);
      mQuit = Add<TextButton>(buttonOptions);

      UIFrame& fLoad = load->GetFrame();
      UIFrame& fSave = mSave->GetFrame();
      UIFrame& fSaveAs = saveAs->GetFrame();
      UIFrame& fDiscard = discard->GetFrame();
      UIFrame& fQuit = mQuit->GetFrame();
      
      root->AddConstraints({
         fLoad.left == mFrame.left + 8,
         fLoad.right == mFrame.right - 8,
         fLoad.top == mFrame.top - 8,
         fLoad.height == 32,

         fSave.left == fLoad.left,
         fSave.right == fLoad.right,
         fSave.top == fLoad.bottom - 8,
         fSave.height == fLoad.height,

         fSaveAs.left == fSave.left,
         fSaveAs.right == fSave.right,
         fSaveAs.top == fSave.bottom - 8,
         fSaveAs.height == fSave.height,

         fDiscard.left == fSaveAs.left,
         fDiscard.right == fSaveAs.right,
         fDiscard.top == fSaveAs.bottom - 8,
         fDiscard.height == fSaveAs.height,

         fQuit.left == fDiscard.left,
         fQuit.right == fDiscard.right,
         fQuit.top == fDiscard.bottom - 8,
         fQuit.height == fDiscard.height,
      });
   }

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

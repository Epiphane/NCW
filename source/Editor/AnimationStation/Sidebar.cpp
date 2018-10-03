// By Thomas Steinke

#include <fstream>
#include <Engine/Core/File.h>
#include <Engine/Core/Window.h>
#include <Shared/Helpers/Asset.h>

#include "../UI/Image.h"
#include "../UI/TextButton.h"

#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

using Game::AnimatedSkeleton;

Sidebar::Sidebar(
   Bounded& parent,
   const Options& options
)
   : SubWindow(parent, options)
   , mFilename(Paths::Normalize(Asset::Animation("player.json")))
{
   {
      // Backdrop
      Image::Options imageOptions;
      imageOptions.z = 0.5f;
      imageOptions.filename = Asset::Image("EditorSidebar.png");
      Add<Image>(imageOptions);
   }

   {
      // Labels
      TextButton::Options buttonOptions;
      buttonOptions.x = 8.0f / GetWidth();
      buttonOptions.y = 1.0f - 43.0f / GetHeight();
      buttonOptions.w = 1.0f - 16.0f / GetWidth();
      buttonOptions.h = 35.0f / GetHeight();

      buttonOptions.text = "Load";
      buttonOptions.onClick = std::bind(&Sidebar::LoadNewFile, this);
      Add<TextButton>(buttonOptions);

      buttonOptions.y -= 35.0f / GetHeight();
      buttonOptions.text = "Save";
      buttonOptions.onClick = std::bind(&Sidebar::SaveFile, this);
      mSave = Add<TextButton>(buttonOptions);
      
      buttonOptions.y -= 35.0f / GetHeight();
      buttonOptions.text = "Save As...";
      buttonOptions.onClick = std::bind(&Sidebar::SaveNewFile, this);
      Add<TextButton>(buttonOptions);
      
      buttonOptions.y -= 35.0f / GetHeight();
      buttonOptions.text = "Discard Changes";
      buttonOptions.onClick = std::bind(&Sidebar::DiscardChanges, this);
      Add<TextButton>(buttonOptions);
      
      buttonOptions.y -= 35.0f / GetHeight();
      buttonOptions.text = "Quit";
      buttonOptions.onClick = std::bind(&Sidebar::Quit, this);
      mQuit = Add<TextButton>(buttonOptions);
   }

   Subscribe<Engine::ComponentAddedEvent<Game::AnimatedSkeleton>>(*this);
   Subscribe<SkeletonModifiedEvent>(*this);
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<Game::AnimatedSkeleton>& evt)
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

   Emit<SkeletonLoadedEvent>(mSkeleton);
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

   Emit<SkeletonSavedEvent>(mSkeleton);
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

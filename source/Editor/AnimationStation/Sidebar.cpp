// By Thomas Steinke

#include <fstream>
#include <Engine/Core/File.h>
#include <Engine/Core/Window.h>
#include <Shared/Helpers/Asset.h>

#include "../UI/Image.h"
#include "../UI/Label.h"

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
      Label::Options labelOptions;
      labelOptions.x = 8.0f / GetWidth();
      labelOptions.y = 1.0f - 43.0f / GetHeight();
      labelOptions.w = 1.0f - 16.0f / GetWidth();
      labelOptions.h = 35.0f / GetHeight();

      labelOptions.text = "Load";
      labelOptions.onClick = std::bind(&Sidebar::LoadNewFile, this);
      Add<Label>(labelOptions);

      labelOptions.y -= 35.0f / GetHeight();
      labelOptions.text = "Save";
      labelOptions.onClick = std::bind(&Sidebar::SaveFile, this);
      mSave = Add<Label>(labelOptions);
      
      labelOptions.y -= 35.0f / GetHeight();
      labelOptions.text = "Save As...";
      labelOptions.onClick = std::bind(&Sidebar::SaveNewFile, this);
      Add<Label>(labelOptions);
      
      labelOptions.y -= 35.0f / GetHeight();
      labelOptions.text = "Discard Changes";
      labelOptions.onClick = std::bind(&Sidebar::DiscardChanges, this);
      Add<Label>(labelOptions);
      
      labelOptions.y -= 35.0f / GetHeight();
      labelOptions.text = "Quit";
      labelOptions.onClick = std::bind(&Sidebar::Quit, this);
      mQuit = Add<Label>(labelOptions);
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

   // No transitioning
   for (AnimatedSkeleton::State& state : mSkeleton->states)
   {
      state.transitions.clear();
   }

   Emit<SkeletonLoadedEvent>();
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

   Emit<SkeletonSavedEvent>();
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

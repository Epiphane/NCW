// By Thomas Steinke

#include <fstream>
#include <Engine/Core/File.h>
#include <Engine/Core/Window.h>
#include <Shared/Helpers/Asset.h>

#include "../UI/Label.h"

#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

Sidebar::Sidebar(
   Bounded& parent,
   const Options& options,
   MainState* state
)
   : SubWindow(parent, options)
   , mState(state)
   , mFilename(Paths::Normalize(Asset::Animation("player.json")))
{
   Label::Options labelOptions;
   labelOptions.x = 8.0f / GetWidth();
   labelOptions.y = 1.0f - 43.0f / GetHeight();
   labelOptions.w = 1.0f - 16.0f / GetWidth();
   labelOptions.h = 35.0f / GetHeight();

   labelOptions.text = "Load";
   labelOptions.onClick = std::bind(&Sidebar::LoadNewFile, this);
   mLoad = Add<Label>(labelOptions);
   labelOptions.y -= 35.0f / GetHeight();

   labelOptions.text = "Save";
   labelOptions.onClick = std::bind(&Sidebar::SaveFile, this);
   mSave = Add<Label>(labelOptions);
   labelOptions.y -= 35.0f / GetHeight();

   labelOptions.text = "Save As...";
   labelOptions.onClick = std::bind(&Sidebar::SaveNewFile, this);
   mSaveAs = Add<Label>(labelOptions);
   labelOptions.y -= 35.0f / GetHeight();

   labelOptions.text = "Discard Changes";
   labelOptions.onClick = std::bind(&Sidebar::DiscardChanges, this);
   mDiscard = Add<Label>(labelOptions);
   labelOptions.y -= 35.0f / GetHeight();

   labelOptions.text = "Quit";
   labelOptions.onClick = std::bind(&Sidebar::Quit, this);
   mQuit = Add<Label>(labelOptions);
   labelOptions.y -= 35.0f / GetHeight();

   LoadFile(mFilename);
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
   using Game::AnimatedSkeleton;
   Engine::ComponentHandle<AnimatedSkeleton> skeleton = mState->GetPlayerSkeleton();

   skeleton->Load(filename);
   skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"torso",1.0f}}, Asset::Model("body4.cub"));
   skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"head",1.0f}}, Asset::Model("elf-head-m02.cub"));
   skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"hair",1.0f}}, Asset::Model("elf-hair-m09.cub"));
   skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"left_hand",1.0f}}, Asset::Model("hand2.cub"));
   skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"right_hand",1.0f}}, Asset::Model("hand2.cub"));
   skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"left_foot",1.0f}}, Asset::Model("foot.cub"));
   skeleton->AddModel(AnimatedSkeleton::BoneWeights{{"right_foot",1.0f}}, Asset::Model("foot.cub"));

   SetModified(true);
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
   std::string serialized = mState->GetPlayerSkeleton()->Serialize();
   std::ofstream out(mFilename);
   out << serialized << std::endl;

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

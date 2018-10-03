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

namespace ModelMaker
{

using Game::AnimatedSkeleton;

Sidebar::Sidebar(
   Bounded& parent,
   const Options& options
)
   : SubWindow(parent, options)
   , mFilename(Paths::Normalize(Asset::Model("dummy.cub")))
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

   Subscribe<Engine::ComponentAddedEvent<Game::CubeModel>>(*this);
   Subscribe<ModelModifiedEvent>(*this);
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<Game::CubeModel>& evt)
{
   mModel = evt.component;

   LoadFile(mFilename);
}

void Sidebar::Receive(const ModelModifiedEvent&)
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

   std::string title = "NCW - Model Maker - ";
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
   if (!mModel)
   {
      // Wait until the component exists!
      return;
   }

   // Load

   Emit<ModelLoadedEvent>(mModel);
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
   //std::string serialized = mModel->Serialize();
   //std::ofstream out(mFilename);
   //out << serialized << std::endl;

   Emit<ModelSavedEvent>(mModel);
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

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

namespace AnimationStation
{

using Engine::UIRoot;
using UI::RectFilled;
using UI::TextButton;

Sidebar::Sidebar(UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "AnimationStationSidebar", glm::vec4(0.2, 0.2, 0.2, 1))
   , mFilename(Asset::Skeleton("greatmace.json"))
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
   discard->ConstrainDimensionsTo(mSave);
   discard->ConstrainLeftAlignedTo(mSave);
   mQuit->ConstrainDimensionsTo(discard);
   mQuit->ConstrainLeftAlignedTo(discard);
      
   root->Subscribe<Engine::ComponentAddedEvent<SimpleAnimationController>>(*this);
   root->Subscribe<SkeletonModifiedEvent>(*this);
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<SimpleAnimationController>& evt)
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

   std::stack<std::string> parts;
   std::string currentFile = filename;
   do
   {
      std::string name = Paths::GetFilename(currentFile);

      Maybe<BindingProperty> maybeData = JSONSerializer::DeserializeFile(currentFile);
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
         currentFile = Paths::Join(Paths::GetDirectory(filename), parent) + ".json";
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

void Sidebar::SaveFile()
{
   mSkeleton->UpdateSkeletonStates();

   for (const Engine::ComponentHandle<SkeletonAnimations>& anims : mSkeleton->animations)
   {
      BindingProperty serialized = anims->Serialize();

      for (const auto&[name, animation] : serialized.pairs())
      {
         std::string path = Asset::Animation(Paths::Join(anims->entity, name.GetStringValue() + ".json"));
         Maybe<void> serialized = JSONSerializer::SerializeFile(path, animation);
         if (!serialized)
         {
            LOG_ERROR("Failed saving file %1: %2", path, serialized.Failure().GetMessage());
         }
      }
   }

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

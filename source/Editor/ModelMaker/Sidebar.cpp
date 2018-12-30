// By Thomas Steinke

#include <Engine/Core/File.h>
#include <Engine/Core/Window.h>
#include <Engine/UI/UIStackView.h>

#include <Shared/Helpers/Asset.h>
#include <Shared/UI/Image.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/SubFrame.h>

#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

using Engine::UIElement;
using UI::Image;
using UI::TextButton;
using UI::RectFilled;

Sidebar::Sidebar(Engine::UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "AnimationStationDock", glm::vec4(0.2, 0.2, 0.2, 1))
   , mFilename(Paths::Normalize(Asset::Model("test.cub")))
   // , mFilename(Paths::Normalize(Asset::Model("elf-head-m02.cub")))
{
   root->Subscribe<Engine::ComponentAddedEvent<CubeModel>>(*this);
   root->Subscribe<ModelModifiedEvent>(*this);

   RectFilled* foreground = Add<RectFilled>("ModelMakerSidebarFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainHorizontalCenterTo(this);
   foreground->ConstrainWidthTo(this, -4);
   foreground->ConstrainTopAlignedTo(this, 2);

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

   buttons->ConstrainTopAlignedTo(foreground);
   buttons->ConstrainHorizontalCenterTo(foreground);
   buttons->ConstrainWidthTo(foreground, -12);
   buttons->ConstrainBottomAlignedTo(foreground, 8);
   load->ConstrainLeftAlignedTo(buttons, 2);
   load->ConstrainWidthTo(buttons, -4);
   load->ConstrainHeight(32);
   mSave->ConstrainDimensionsTo(load);
   mSave->ConstrainLeftAlignedTo(load);
   saveAs->ConstrainDimensionsTo(mSave);
   saveAs->ConstrainLeftAlignedTo(mSave);
   discard->ConstrainDimensionsTo(saveAs);
   discard->ConstrainLeftAlignedTo(saveAs);

   // Create a scrollable list of available models
   UI::SubFrame* explorer = Add<UI::SubFrame>();

   explorer->ConstrainBelow(foreground, 2);
   explorer->ConstrainWidthTo(foreground);
   explorer->ConstrainLeftAlignedTo(foreground);
   explorer->ConstrainBottomAlignedTo(this);
   std::vector<std::string> testData = {
      "dummy.cub", "aim.cub", "barrel.cub", "bed.cub",
      "angry.cub", "anvil.cub", "big-door.cub", "biscuit-role.cub",
      "bomb1.cub", "boat.cub", "body1.cub", "body2.cub", "body3.cub",
      "body4.cub", "bow.cub", "bowl1.cub", "boot.cub"
   };
   TextButton* prevButton = nullptr;
   for (const std::string& file : testData)
   {
      buttonOptions.text = file;
      buttonOptions.onClick = std::bind(&Sidebar::LoadFile, this, Asset::Model(file));
      TextButton* button = explorer->Add<TextButton>(buttonOptions);

      if (prevButton == nullptr)
      {
         button->ConstrainWidthTo(&explorer->GetUI());
         button->ConstrainHeight(32);
         button->ConstrainTopAlignedTo(&explorer->GetUI());
         button->ConstrainLeftAlignedTo(&explorer->GetUI(), 8);
      }
      else
      {
         button->ConstrainDimensionsTo(prevButton);
         button->ConstrainBelow(prevButton, 0);
         button->ConstrainLeftAlignedTo(prevButton);
      }
      prevButton = button;
   }
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<CubeModel>& evt)
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
   mModel->Load(filename);
   mpRoot->Emit<ModelLoadedEvent>(mModel);
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

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
